/*
 * KRR hyperparameter optimization with MPI
 *
 * 
 * Author Linus Lind: MIT License
 *
 * Rank 0 : 1-D search over lambda (fixed sigma=5.0, intentionally wrong), sends best lambda to rank 1
 * Rank 1 : waits for best lambda, then 1-D search over sigma, saves results_sequential.txt
 * Rank 2 : independent 2-D grid search, saves results_grid.txt
 *
 * Compile : mpicc krr_mpi.c -o krr_mpi -lm
 * Run     : mpirun -n 3 ./krr_mpi
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

#define N_FOLDS    5
#define N_LAM     24
#define N_SIG     24
#define FIXED_SIG  200.0   /* deliberately wide — 10x true sigma, so sequential search is visibly suboptimal */
#define N_EVAL   500     /* points in the dense evaluation grid */
#define X_LO    -12.0
#define X_HI     12.0

/* ------------------------------------------------------------------ */
/* Data                                                                 */
/* ------------------------------------------------------------------ */

typedef struct { double *X; double *y; int n, d; } Dataset;

static Dataset load_data(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Cannot open %s\n", path);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    Dataset ds;
    fscanf(f, "%d %d", &ds.n, &ds.d);
    ds.X = malloc(ds.n * ds.d * sizeof(double));
    ds.y = malloc(ds.n * sizeof(double));
    for (int i = 0; i < ds.n; i++) {
        for (int j = 0; j < ds.d; j++)
            fscanf(f, "%lf", &ds.X[i * ds.d + j]);
        fscanf(f, "%lf", &ds.y[i]);
    }
    fclose(f);
    return ds;
}

/* ------------------------------------------------------------------ */
/* RBF kernel  K[i,j] = exp(-||X1[i]-X2[j]||^2 / (2 sigma^2))        */
/* ------------------------------------------------------------------ */

static void rbf_kernel(const double *X1, int n1,
                        const double *X2, int n2,
                        int d, double sigma, double *K) {
    double inv2s2 = 1.0 / (2.0 * sigma * sigma);
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < n2; j++) {
            double sq = 0.0;
            for (int k = 0; k < d; k++) {
                double diff = X1[i*d+k] - X2[j*d+k];
                sq += diff * diff;
            }
            K[i*n2+j] = exp(-sq * inv2s2);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Solve (A + lambda*I) x = b via Cholesky decomposition              */
/* A is n x n symmetric positive semi-definite, stored row-major      */
/* ------------------------------------------------------------------ */

static void cholesky_solve(const double *A, int n, double lambda,
                            const double *b, double *x) {
    double *L = calloc(n * n, sizeof(double));

    /* Cholesky factorisation: (A + (lambda + eps)*I) = L L^T */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            double s = A[i*n+j] + (i == j ? lambda + 1e-10 : 0.0);
            for (int k = 0; k < j; k++)
                s -= L[i*n+k] * L[j*n+k];
            L[i*n+j] = (i == j) ? sqrt(s) : s / L[j*n+j];
        }
    }

    /* Forward substitution: L z = b */
    double *z = malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        double s = b[i];
        for (int k = 0; k < i; k++)
            s -= L[i*n+k] * z[k];
        z[i] = s / L[i*n+i];
    }

    /* Backward substitution: L^T x = z */
    for (int i = n-1; i >= 0; i--) {
        double s = z[i];
        for (int k = i+1; k < n; k++)
            s -= L[k*n+i] * x[k];
        x[i] = s / L[i*n+i];
    }

    free(L);
    free(z);
}

/* ------------------------------------------------------------------ */
/* K-fold cross-validation MSE                                         */
/* ------------------------------------------------------------------ */

static double cv_mse(const double *X, const double *y, int n, int d,
                     double lambda, double sigma) {
    int fold_size = n / N_FOLDS;
    double total_mse = 0.0;

    int n_tr_max = n - fold_size;
    double *X_tr  = malloc(n_tr_max * d * sizeof(double));
    double *y_tr  = malloc(n_tr_max * sizeof(double));
    double *X_val = malloc(fold_size * d * sizeof(double));
    double *y_val = malloc(fold_size * sizeof(double));

    for (int fold = 0; fold < N_FOLDS; fold++) {
        int vs = fold * fold_size;   /* val start */
        int ve = vs + fold_size;     /* val end   */
        int n_val = fold_size;
        int n_tr  = n - n_val;

        /* split */
        int ti = 0;
        for (int i = 0; i < n; i++) {
            if (i >= vs && i < ve) {
                int vi = i - vs;
                memcpy(X_val + vi*d, X + i*d, d * sizeof(double));
                y_val[vi] = y[i];
            } else {
                memcpy(X_tr + ti*d, X + i*d, d * sizeof(double));
                y_tr[ti] = y[i];
                ti++;
            }
        }

        /* training kernel */
        double *K_tr = malloc(n_tr * n_tr * sizeof(double));
        rbf_kernel(X_tr, n_tr, X_tr, n_tr, d, sigma, K_tr);

        /* dual coefficients: alpha = (K + lambda I)^{-1} y */
        double *alpha = malloc(n_tr * sizeof(double));
        cholesky_solve(K_tr, n_tr, lambda, y_tr, alpha);

        /* cross kernel and predictions */
        double *K_val = malloc(n_val * n_tr * sizeof(double));
        rbf_kernel(X_val, n_val, X_tr, n_tr, d, sigma, K_val);

        double mse = 0.0;
        for (int i = 0; i < n_val; i++) {
            double pred = 0.0;
            for (int j = 0; j < n_tr; j++)
                pred += K_val[i*n_tr+j] * alpha[j];
            double err = y_val[i] - pred;
            mse += err * err;
        }
        total_mse += mse / n_val;

        free(K_tr); free(alpha); free(K_val);
    }

    free(X_tr); free(y_tr); free(X_val); free(y_val);
    return total_mse / N_FOLDS;
}

/* ------------------------------------------------------------------ */
/* Fit on full data, write training predictions into y_pred            */
/* ------------------------------------------------------------------ */

static void fit_and_predict(const double *X, const double *y, int n, int d,
                             double lambda, double sigma, double *y_pred) {
    double *K = malloc(n * n * sizeof(double));
    rbf_kernel(X, n, X, n, d, sigma, K);

    double *alpha = malloc(n * sizeof(double));
    cholesky_solve(K, n, lambda, y, alpha);

    for (int i = 0; i < n; i++) {
        y_pred[i] = 0.0;
        for (int j = 0; j < n; j++)
            y_pred[i] += K[i*n+j] * alpha[j];
    }

    free(K);
    free(alpha);
}

static void save_predictions(const char *path,
                              const double *X, int d,
                              const double *y_true,
                              const double *y_pred, int n) {
    FILE *f = fopen(path, "w");
    for (int k = 0; k < d; k++) fprintf(f, "x%d ", k + 1);
    fprintf(f, "y_true y_pred\n");
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < d; k++)
            fprintf(f, "%.10f ", X[i*d+k]);
        fprintf(f, "%.10f %.10f\n", y_true[i], y_pred[i]);
    }
    fclose(f);
}

/* ------------------------------------------------------------------ */
/* Predict on a dense 1-D evaluation grid and save to file            */
/* ------------------------------------------------------------------ */

static void save_eval_grid(const char *path,
                            const double *X_train, const double *y_train,
                            int n_train, int d,
                            double lambda, double sigma) {
    /* Fit alpha on full training set */
    double *K_tr = malloc(n_train * n_train * sizeof(double));
    rbf_kernel(X_train, n_train, X_train, n_train, d, sigma, K_tr);
    double *alpha = malloc(n_train * sizeof(double));
    cholesky_solve(K_tr, n_train, lambda, y_train, alpha);
    free(K_tr);

    /* Dense evaluation grid (1-D: each point is a length-d=1 row) */
    double *X_eval = malloc(N_EVAL * d * sizeof(double));
    double step = (X_HI - X_LO) / (N_EVAL - 1);
    for (int i = 0; i < N_EVAL; i++)
        for (int k = 0; k < d; k++)
            X_eval[i*d+k] = X_LO + i * step;   /* all features share same range */

    double *K_eval = malloc(N_EVAL * n_train * sizeof(double));
    rbf_kernel(X_eval, N_EVAL, X_train, n_train, d, sigma, K_eval);

    FILE *f = fopen(path, "w");
    for (int k = 0; k < d; k++) fprintf(f, "x%d ", k + 1);
    fprintf(f, "y_pred\n");
    for (int i = 0; i < N_EVAL; i++) {
        double pred = 0.0;
        for (int j = 0; j < n_train; j++)
            pred += K_eval[i*n_train+j] * alpha[j];
        for (int k = 0; k < d; k++)
            fprintf(f, "%.10f ", X_eval[i*d+k]);
        fprintf(f, "%.10f\n", pred);
    }
    fclose(f);

    free(X_eval);
    free(K_eval);
    free(alpha);
}

/* ------------------------------------------------------------------ */
/* Grid helpers                                                         */
/* ------------------------------------------------------------------ */

static void logspace(double log10_lo, double log10_hi, int n, double *out) {
    double step = (log10_hi - log10_lo) / (n - 1);
    for (int i = 0; i < n; i++)
        out[i] = pow(10.0, log10_lo + i * step);
}

/* ------------------------------------------------------------------ */
/* Main                                                                 */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, ntasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    if (ntasks < 3) {
        if (rank == 0) fprintf(stderr, "Need at least 3 MPI tasks.\n");
        MPI_Finalize();
        return 1;
    }

    Dataset ds = load_data("krr_data.txt");

    double lgrid[N_LAM], sgrid[N_SIG];
    logspace(-4.0,  1.0, N_LAM, lgrid);   /* lambda: 1e-4 .. 10  */
    logspace(-1.0,  2.0, N_SIG, sgrid);   /* sigma:  0.1  .. 100 */

    /* ----------------------------------------------------------------
     * Rank 0 : 1-D search over lambda (sigma fixed at FIXED_SIG)
     * ---------------------------------------------------------------- */
    if (rank == 0) {
        double best_lam = lgrid[0], best_mse = 1e300;
        for (int i = 0; i < N_LAM; i++) {
            double mse = cv_mse(ds.X, ds.y, ds.n, ds.d, lgrid[i], FIXED_SIG);
            printf("Rank 0: lambda=%.4e  CV-MSE=%.6f\n", lgrid[i], mse);
            if (mse < best_mse) { best_mse = mse; best_lam = lgrid[i]; }
        }
        printf("Rank 0: best lambda = %.4e (MSE=%.6f)\n", best_lam, best_mse);
        MPI_Send(&best_lam, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);

    /* ----------------------------------------------------------------
     * Rank 1 : wait for best lambda from rank 0, then search sigma
     * ---------------------------------------------------------------- */
    } else if (rank == 1) {
        double best_lam;
        MPI_Recv(&best_lam, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank 1: received lambda=%.4e, searching sigma ...\n", best_lam);

        double best_sig = sgrid[0], best_mse = 1e300;
        for (int j = 0; j < N_SIG; j++) {
            double mse = cv_mse(ds.X, ds.y, ds.n, ds.d, best_lam, sgrid[j]);
            printf("Rank 1: sigma=%.4e  CV-MSE=%.6f\n", sgrid[j], mse);
            if (mse < best_mse) { best_mse = mse; best_sig = sgrid[j]; }
        }
        printf("Rank 1: best sigma = %.4e (MSE=%.6f)\n", best_sig, best_mse);

        FILE *f = fopen("results_sequential.txt", "w");
        fprintf(f, "lambda %.6e\nsigma  %.6e\nmse    %.6f\n",
                best_lam, best_sig, best_mse);
        fclose(f);

        double *y_pred = malloc(ds.n * sizeof(double));
        fit_and_predict(ds.X, ds.y, ds.n, ds.d, best_lam, best_sig, y_pred);
        save_predictions("predictions_sequential.txt",
                         ds.X, ds.d, ds.y, y_pred, ds.n);
        free(y_pred);
        save_eval_grid("eval_sequential.txt",
                       ds.X, ds.y, ds.n, ds.d, best_lam, best_sig);
        printf("Rank 1: predictions written to predictions_sequential.txt / eval_sequential.txt\n");

    /* ----------------------------------------------------------------
     * Rank 2 : full 2-D grid search (independent)
     * ---------------------------------------------------------------- */
    } else if (rank == 2) {
        double best_lam = lgrid[0], best_sig = sgrid[0], best_mse = 1e300;
        for (int i = 0; i < N_LAM; i++) {
            for (int j = 0; j < N_SIG; j++) {
                double mse = cv_mse(ds.X, ds.y, ds.n, ds.d, lgrid[i], sgrid[j]);
                printf("Rank 2: lambda=%.4e sigma=%.4e  CV-MSE=%.6f\n",
                       lgrid[i], sgrid[j], mse);
                if (mse < best_mse) {
                    best_mse = mse;
                    best_lam = lgrid[i];
                    best_sig = sgrid[j];
                }
            }
        }
        printf("Rank 2: best lambda=%.4e sigma=%.4e (MSE=%.6f)\n",
               best_lam, best_sig, best_mse);

        FILE *f = fopen("results_grid.txt", "w");
        fprintf(f, "lambda %.6e\nsigma  %.6e\nmse    %.6f\n",
                best_lam, best_sig, best_mse);
        fclose(f);

        double *y_pred = malloc(ds.n * sizeof(double));
        fit_and_predict(ds.X, ds.y, ds.n, ds.d, best_lam, best_sig, y_pred);
        save_predictions("predictions_grid.txt",
                         ds.X, ds.d, ds.y, y_pred, ds.n);
        free(y_pred);
        save_eval_grid("eval_grid.txt",
                       ds.X, ds.y, ds.n, ds.d, best_lam, best_sig);
        printf("Rank 2: predictions written to predictions_grid.txt / eval_grid.txt\n");
    }

    free(ds.X);
    free(ds.y);
    MPI_Finalize();
    return 0;
}
