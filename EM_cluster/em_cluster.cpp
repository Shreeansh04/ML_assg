#include <bits/stdc++.h>

struct Point
{
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};

struct Gaussian
{
    double meanX, meanY, covXX, covYY, covXY, weight;

    Gaussian() : meanX(0), meanY(0), covXX(1), covYY(1), covXY(0), weight(1) {}
};

// Function to calculate the probability of a point belonging to a cluster
double calculateProbability(const Point &point, const Gaussian &cluster)
{
    double det = cluster.covXX * cluster.covYY - cluster.covXY * cluster.covXY;
    double dx = point.x - cluster.meanX;
    double dy = point.y - cluster.meanY;

    double exponent = -0.5 / det * (cluster.covYY * dx * dx - 2 * cluster.covXY * dx * dy + cluster.covXX * dy * dy);
    return cluster.weight / (2 * M_PI * std::sqrt(det)) * std::exp(exponent);
}

// Expectation step: Assign each point to the most probable cluster
void expectationStep(const std::vector<Point> &data, const std::vector<Gaussian> &clusters, std::vector<int> &assignments)
{
    for (int i = 0; i < data.size(); i++)
    {
        double maxProbability = 0;
        int bestCluster = 0;

        for (int j = 0; j < clusters.size(); j++)
        {
            double probability = calculateProbability(data[i], clusters[j]);
            if (probability > maxProbability)
            {
                maxProbability = probability;
                bestCluster = j;
            }
        }

        assignments[i] = bestCluster;
    }
}

// Maximization step: Update cluster parameters based on assigned points
void maximizationStep(const std::vector<Point> &data, const std::vector<int> &assignments, std::vector<Gaussian> &clusters)
{
    for (int j = 0; j < clusters.size(); j++)
    {
        double sumWeights = 0;
        double sumX = 0, sumY = 0;
        double sumXX = 0, sumYY = 0, sumXY = 0;

        for (int i = 0; i < data.size(); i++)
        {
            if (assignments[i] == j)
            {
                sumWeights += 1;
                sumX += data[i].x;
                sumY += data[i].y;
                sumXX += data[i].x * data[i].x;
                sumYY += data[i].y * data[i].y;
                sumXY += data[i].x * data[i].y;
            }
        }

        if (sumWeights > 0)
        {
            clusters[j].weight = sumWeights / data.size();
            clusters[j].meanX = sumX / sumWeights;
            clusters[j].meanY = sumY / sumWeights;
            clusters[j].covXX = sumXX / sumWeights - clusters[j].meanX * clusters[j].meanX;
            clusters[j].covYY = sumYY / sumWeights - clusters[j].meanY * clusters[j].meanY;
            clusters[j].covXY = sumXY / sumWeights - clusters[j].meanX * clusters[j].meanY;
        }
    }
}

// EM clustering algorithm
void emClustering(const std::vector<Point> &data, int numClusters, int maxIterations)
{
    // Initialize clusters randomly
    std::vector<Gaussian> clusters(numClusters);
    for (auto &cluster : clusters)
    {
        cluster.meanX = static_cast<double>(rand()) / INT_MAX * 10; // range of data is kept from 0 to 10
        cluster.meanY = static_cast<double>(rand()) / INT_MAX * 10;
    }

    // Assignments vector to keep track of the cluster each point belongs to
    std::vector<int> assignments(data.size(), 0);

    // EM iterations
    for (int iteration = 0; iteration < maxIterations; ++iteration)
    {
        // Expectation step
        expectationStep(data, clusters, assignments);

        // Maximization step
        maximizationStep(data, assignments, clusters);
    }

    // Print final cluster parameters
    for (size_t i = 0; i < clusters.size(); ++i)
    {
        std::cout << "Cluster " << i << ": Mean=(" << clusters[i].meanX << ", " << clusters[i].meanY << "), "
                  << "Covariance=(" << clusters[i].covXX << ", " << clusters[i].covYY << ", " << clusters[i].covXY << "), "
                  << "Weight=" << clusters[i].weight << "\n";
    }
}

int main()
{
    // Generate some random 2D data points for testing
    std::vector<Point> data;
    for (int i = 0; i < 100; ++i)
    {
        data.emplace_back(static_cast<double>(rand()) / INT_MAX * 20, static_cast<double>(rand()) / INT_MAX * 20);
    }

    // Specify the number of clusters and maximum iterations
    int numClusters = 3;
    int maxIterations = 100;

    // Run EM clustering
    emClustering(data, numClusters, maxIterations);

    return 0;
}
