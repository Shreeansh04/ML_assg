#include <iostream>
#include <fstream>
#include <vector>
#include<sstream>
#include <random>

struct LinearRegression {
    double intercept;
    double slope;
};

// Function to calculate the mean of a vector
double mean(const std::vector<double>& data) {
    double sum = 0.0;
    for (double value : data) {
        sum += value;
    }
    return sum / data.size();
}

// Generate random numbers using normal distribution
double randomNormal(double mean, double stddev) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::normal_distribution<double> dist(mean, stddev);
    return dist(gen);
}
double predictPrice(const LinearRegression& regression, double age) {
    return regression.intercept + regression.slope * age;
}
// Function to perform Metropolis-Hastings algorithm for Bayesian Linear Regression
LinearRegression bayesianLinearRegression(const std::vector<double>& x, const std::vector<double>& y) {
    LinearRegression result;

    double xMean = mean(x);
    double yMean = mean(y);
    double sumXY = 0.0, sumX2 = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        sumXY += (x[i] - xMean) * (y[i] - yMean);
        sumX2 += (x[i] - xMean) * (x[i] - xMean);
    }

    // Prior hyperparameters
    double priorMeanSlope = 0.0;
    double priorStdDevSlope = 10.0;
    double priorMeanIntercept = 0.0;
    double priorStdDevIntercept = 10.0;

    double slope = randomNormal(priorMeanSlope, priorStdDevSlope);
    double intercept = randomNormal(priorMeanIntercept, priorStdDevIntercept);

    double priorLikelihood = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        priorLikelihood += 0.5 * (y[i] - (intercept + slope * x[i])) * (y[i] - (intercept + slope * x[i]));
    }

    double acceptanceRate = 0.0;
    int iterations = 10000; // Number of iterations

    for (int i = 0; i < iterations; ++i) {
        double newSlope = randomNormal(slope, priorStdDevSlope);
        double newIntercept = randomNormal(intercept, priorStdDevIntercept);

        double newLikelihood = 0.0;
        for (size_t j = 0; j < x.size(); ++j) {
            newLikelihood += 0.5 * (y[j] - (newIntercept + newSlope * x[j])) * (y[j] - (newIntercept + newSlope * x[j]));
        }

        double acceptanceProbability = std::min(1.0, exp(priorLikelihood - newLikelihood));
        double randomValue = (rand() / (RAND_MAX + 1.0));

        if (randomValue < acceptanceProbability) {
            slope = newSlope;
            intercept = newIntercept;
            priorLikelihood = newLikelihood;
            acceptanceRate++;
        }
    }

    result.slope = slope;
    result.intercept = intercept;

    std::cout << "Acceptance Rate: " << (acceptanceRate / iterations) * 100 << "%" << std::endl;
    return result;
}

int main() {
    // Read the data from the CSV file
    std::ifstream file("house_data.csv"); // Replace 'house_data.csv' with your file name
    if (!file.is_open()) {
        std::cout << "Error opening the file!" << std::endl;
        return 1;
    }

    std::vector<double> ages;
    std::vector<double> prices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string value1, value2;
        if (std::getline(iss, value1, ',') && std::getline(iss, value2, ',')) {
            try {
                ages.push_back(std::stod(value1));
                prices.push_back(std::stod(value2));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid argument: " << e.what() << ". Please check the input data." << std::endl;
                return 1;
            }
        }
    }

    file.close();

    // Bayesian Linear Regression
    LinearRegression regression = bayesianLinearRegression(ages, prices);

    std::cout << "Bayesian Linear Regression:\n";
    std::cout << "Intercept: " << regression.intercept << "\nSlope: " << regression.slope << std::endl;

     double userHouseAge;
    std::cout << "Enter the age of the house to predict its price: ";
    std::cin >> userHouseAge;

    // Predict house price
    double predictedPrice = predictPrice(regression, userHouseAge);
    std::cout << "Predicted house price for age " << userHouseAge << " is: " << predictedPrice << std::endl;


    return 0;
}
