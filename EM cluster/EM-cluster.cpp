


#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <ctime>
#include "gnuplot-iostream.h"  

using namespace std;

// Define the number of clusters (K) to be used in the EM clustering algorithm
const int K = 3;
// Define the number of features in the wine dataset
const int num_features = 13;

// Define the number of data points in the wine dataset
const int num_data_points = 178;

// Delaring 2 Data structures to hold the wine dataset and cluster probabilities
vector<vector<double>> data(num_data_points, vector<double>(num_features, 0.0));
vector<vector<double>> cluster_probabilities(num_data_points, vector<double>(K, 0.0));

// Function to initialize cluster means(randomly) and covariances
void initialize_clusters(vector<vector<double>>& cluster_means, vector<vector<vector<double>>>& cluster_covariances) {
    // Initialize cluster means randomly (similar to K-means)
    for (int k = 0; k < K; k++) {
        for (int f = 0; f < num_features; f++) {
            cluster_means[k][f] = (rand() % 1000) / 1000.0; // Random initialization between 0 and 1
        }
    }

    // Initialize cluster covariances as identity matrices
    for (int k = 0; k < K; k++) {
        for (int i = 0; i < num_features; i++) {
            for (int j = 0; j < num_features; j++) {
                cluster_covariances[k][i][j] = (i == j) ? 1.0 : 0.0;
            }
        }
    }
}

// Function to calculate the probability density of a data point for a given cluster
double calculate_probability_density(const vector<double>& data_point, const vector<double>& cluster_mean, const vector<vector<double>>& cluster_covariance) {
    // Calculate the Mahalanobis distance
    vector<double> diff(num_features);
    for (int f = 0; f < num_features; f++) {
        diff[f] = data_point[f] - cluster_mean[f];
    }

    double mahalanobis_distance = 0.0;
    for (int i = 0; i < num_features; i++) {
        for (int j = 0; j < num_features; j++) {
            mahalanobis_distance += diff[i] * cluster_covariance[i][j] * diff[j];
        }
    }

    // Calculate the probability density using the multivariate Gaussian distribution
    double determinant = 1.0;
    for (int f = 0; f < num_features; f++) {
        determinant *= cluster_covariance[f][f];
    }

    double density = 1.0 / (sqrt(pow(2.0 * M_PI, num_features) * determinant)) * exp(-0.5 * mahalanobis_distance);
    return density;
}

// Function to perform the E-step (Expectation step) 
//It calculates the probability of each data point belonging to each cluster using the calculate_probability_density function and then normalizes the cluster probabilities.
void expectation_step(const std::vector<std::vector<double>>& cluster_means, const std::vector<std::vector<std::vector<double>>>& cluster_covariances) {
    for (int i = 0; i < num_data_points; i++) {
        double sum_probabilities = 0.0;
        for (int k = 0; k < K; k++) {
            cluster_probabilities[i][k] = calculate_probability_density(data[i], cluster_means[k], cluster_covariances[k]);
            sum_probabilities += cluster_probabilities[i][k];
        }

        // Normalize the cluster probabilities
        for (int k = 0; k < K; k++) {
            cluster_probabilities[i][k] /= sum_probabilities;
        }
    }
}

// Function to perform the M-step (Maximization step)
// It calculates the new cluster means and covariances based on the cluster probabilities computed in the E-step. 
//The calculations are based on the weighted averages of data points within each cluster.

void maximization_step(vector<vector<double>>& cluster_means, vector<vector<vector<double>>>& cluster_covariances) {
    for (int k = 0; k < K; k++) {
        double Nk = 0.0;
        vector<double> Nk_feature_sum(num_features, 0.0);
        vector<vector<double>> Nk_covariance_sum(num_features, vector<double>(num_features, 0.0));

        for (int i = 0; i < num_data_points; i++) {
            Nk += cluster_probabilities[i][k];

            for (int f = 0; f < num_features; f++) {
                Nk_feature_sum[f] += cluster_probabilities[i][k] * data[i][f];
            }
        }

        for (int i = 0; i < num_data_points; i++) {
            for (int f1 = 0; f1 < num_features; f1++) {
                for (int f2 = 0; f2 < num_features; f2++) {
                    Nk_covariance_sum[f1][f2] += cluster_probabilities[i][k] * (data[i][f1] - cluster_means[k][f1]) * (data[i][f2] - cluster_means[k][f2]);
                }
            }
        }

        for (int f = 0; f < num_features; f++) {
            cluster_means[k][f] = Nk_feature_sum[f] / Nk;
        }

        for (int f1 = 0; f1 < num_features; f1++) {
            for (int f2 = 0; f2 < num_features; f2++) {
                cluster_covariances[k][f1][f2] = Nk_covariance_sum[f1][f2] / Nk;
            }
        }
    }
}

// Function to calculate the log-likelihood of the data
//The log-likelihood is used to monitor the convergence of the EM algorithm. 
//It computes the sum of the log cluster probabilities for each data point.

double log_likelihood() {
    double log_likelihood = 0.0;
    for (int i = 0; i < num_data_points; i++) {
        double sum_probabilities = 0.0;
        for (int k = 0; k < K; k++) {
            sum_probabilities += cluster_probabilities[i][k];
        }
        log_likelihood += log(sum_probabilities);
    }
    return log_likelihood;
}

int main() {
    // Seed the random number generator
    srand(time(0));

    // Read the wine dataset from a file
    ifstream data_file("wine.data");
    for (int i = 0; i < num_data_points; i++) {
        for (int f = 0; f < num_features; f++) {
            data_file >> data[i][f];
        }
    }

    // Initialize cluster means and covariances
    vector<vector<double>> cluster_means(K, vector<double>(num_features, 0.0));
    vector<vector<vector<double>>> cluster_covariances(K, vector<vector<double>>(num_features, vector<double>(num_features, 0.0)));
    initialize_clusters(cluster_means, cluster_covariances);

    // Create a Gnuplot object
    Gnuplot gp("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\"");

    // EM algorithm iterations
    const int max_iterations = 100;
    const double tolerance = 1e-6;
    double prev_log_likelihood = 0.0;

    for (int iter = 0; iter < max_iterations; iter++) {
        // E-step
        expectation_step(cluster_means, cluster_covariances);

        // M-step
        maximization_step(cluster_means, cluster_covariances);

        // Calculate and print log likelihood
        double curr_log_likelihood = log_likelihood();
        cout << "Iteration " << iter << ": Log Likelihood = " << curr_log_likelihood << endl;

        // Check for convergence
        if (fabs(curr_log_likelihood - prev_log_likelihood) < tolerance) {
            break;
        }

        prev_log_likelihood = curr_log_likelihood;

        // Plot the clusters
        vector<vector<double>> cluster_data(K);
        for (int i = 0; i < num_data_points; i++) {
            int max_cluster = 0;
            double max_probability = cluster_probabilities[i][0];
            for (int k = 1; k < K; k++) {
                if (cluster_probabilities[i][k] > max_probability) {
                    max_probability = cluster_probabilities[i][k];
                    max_cluster = k;
                }
            }
            cluster_data[max_cluster].push_back(data[i][0]); // Change this to the appropriate feature to plot
        }

        for (int k = 0; k < K; k++) {
            gp << "plot '-' with points title 'Cluster " << k << "'\n";
            gp.send1d(boost::make_tuple(cluster_data[k]));
        }
    }

    return 0;
}
