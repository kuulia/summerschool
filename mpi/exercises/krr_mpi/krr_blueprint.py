
import numpy as np


def cholesky_inv(matrix):
    '''
    Inverts matrix using Cholesky decomposition,
    adds small number 1e-10 to diagonal elements for numerical stability
    '''
    M = matrix + np.eye(max(matrix.shape))*1e-10
    L = np.linalg.cholesky(M)
    inv_L = np.linalg.solve(L, np.eye(L.shape[0]))
    return inv_L.T @ inv_L


def rbf_kernel(X1, X2, sigma=1.0):
    """
    Compute RBF (Gaussian) kernel matrix.
    K[i,j] = exp(-||X1[i] - X2[j]||^2 / (2 * sigma^2))
    """
    diff = X1[:, np.newaxis, :] - X2[np.newaxis, :, :]  # (n1, n2, d)
    sq_dists = np.sum(diff ** 2, axis=-1)                # (n1, n2)
    return np.exp(-sq_dists / (2.0 * sigma ** 2))


def ridge_obj(alpha, K, y, lambda_val) -> float:
    """
    Kernel ridge regression objective:
      alpha^T (K^2 + lambda*K) alpha - 2 y^T K alpha
    """
    a = alpha
    L = lambda_val
    return a.T @ (K @ K.T + L * K) @ a - 2 * y.T @ K @ a


def analytical_solution(K, lambda_val, y):
    """
    Compute dual coefficients: alpha = (K + lambda*I)^{-1} y
    """
    I = np.eye(K.shape[0])
    return cholesky_inv(K + lambda_val * I) @ y


def train(X_train, y_train, lambda_val, sigma=1.0):
    """
    Build the kernel matrix and solve for dual coefficients alpha.
    Returns (K, alpha).
    """
    K = rbf_kernel(X_train, X_train, sigma)
    alpha = analytical_solution(K, lambda_val, y_train)
    return K, alpha


def predict(X_test, X_train, alpha, sigma=1.0):
    """
    Predict on new test points: f(X_test) = K(X_test, X_train) @ alpha
    """
    K_test = rbf_kernel(X_test, X_train, sigma)
    return K_test @ alpha


def cross_val_mse(X, y, lambda_val, sigma, n_folds=5):
    """
    K-fold cross-validation MSE for a single (lambda_val, sigma) pair.
    """
    n = len(y)
    indices = np.arange(n)
    fold_size = n // n_folds
    mse_sum = 0.0

    for fold in range(n_folds):
        val_idx = indices[fold * fold_size:(fold + 1) * fold_size]
        train_idx = np.concatenate([indices[:fold * fold_size],
                                    indices[(fold + 1) * fold_size:]])

        X_tr, y_tr = X[train_idx], y[train_idx]
        X_val, y_val = X[val_idx], y[val_idx]

        _, alpha = train(X_tr, y_tr, lambda_val, sigma)
        y_pred = predict(X_val, X_tr, alpha, sigma)
        mse_sum += np.mean((y_val - y_pred) ** 2)

    return mse_sum / n_folds


def generate_data(n_samples=100, n_features=1, sigma=1.0, noise_std=0.1,
                  x_range=(-5.0, 5.0), seed=None):
    """
    Generate noisy training data suited for the RBF kernel by sampling a
    latent function from a Gaussian Process with an RBF covariance, then
    adding i.i.d. Gaussian noise.

    The GP prior matches the kernel, so the model is correctly specified:
    the signal lives exactly in the RKHS the RBF kernel defines.

    Parameters
    ----------
    n_samples  : number of data points
    n_features : input dimensionality
    sigma      : RBF kernel width used for the GP covariance
    noise_std  : standard deviation of additive observation noise
    x_range    : (low, high) for uniform input sampling per feature
    seed       : random seed for reproducibility

    Returns
    -------
    X : (n_samples, n_features) input array
    y : (n_samples,) noisy target array
    """
    rng = np.random.default_rng(seed)
    X = rng.uniform(x_range[0], x_range[1], size=(n_samples, n_features))
    K = rbf_kernel(X, X, sigma)
    # sample latent function from GP prior
    f = rng.multivariate_normal(np.zeros(n_samples), K)
    y = f + rng.normal(0.0, noise_std, size=n_samples)
    return X, y


def save_data(X, y, path="krr_data.txt"):
    """
    Save dataset to a whitespace-delimited text file.
    First line: n_samples n_features
    Remaining lines: x1 x2 ... xd y  (one sample per line)
    """
    n, d = X.shape
    with open(path, "w") as f:
        f.write(f"{n} {d}\n")
        for i in range(n):
            row = " ".join(f"{v:.10f}" for v in X[i]) + f" {y[i]:.10f}"
            f.write(row + "\n")


def grid_search(X, y, lambda_grid, sigma_grid, n_folds=5):
    """
    Grid search over lambda_grid x sigma_grid using k-fold CV.
    Returns (best_lambda, best_sigma, scores) where scores[i,j] is the
    CV MSE for lambda_grid[i] and sigma_grid[j].
    """
    scores = np.zeros((len(lambda_grid), len(sigma_grid)))

    for i, lam in enumerate(lambda_grid):
        for j, sig in enumerate(sigma_grid):
            scores[i, j] = cross_val_mse(X, y, lam, sig, n_folds)

    best_i, best_j = np.unravel_index(np.argmin(scores), scores.shape)
    return lambda_grid[best_i], sigma_grid[best_j], scores


if __name__ == "__main__":
    # Larger dataset with a shorter length scale (sigma=0.5) so the latent
    # function is wigglier and the hyperparameter search is more meaningful.
    X, y = generate_data(n_samples=500, n_features=1, sigma=0.5,
                         noise_std=0.1, x_range=(-8.0, 8.0), seed=42)
    save_data(X, y, "krr_data.txt")
    print(f"Saved {len(y)} samples to krr_data.txt")
