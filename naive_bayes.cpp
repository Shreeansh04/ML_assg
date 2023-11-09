#include <iostream>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

class NaiveBayesClassifier
{
private:
    std::map<std::string, int> classCounts;
    std::map<std::string, std::map<std::string, int>> wordCounts;
    std::unordered_set<std::string> vocab;
    double totalDocuments;

public:
    NaiveBayesClassifier()
    {
        totalDocuments = 0;
    }

    void train(const std::string &trainingFile)
    {
        std::ifstream file(trainingFile);
        std::string line;
        while (getline(file, line))
        {
            std::string id, city, className, tweet, word;
            std::stringstream ss(line);
            getline(ss, id, ',');
            getline(ss, city, ',');
            getline(ss, className, ',');
            getline(ss, tweet, ',');

            classCounts[className]++;
            // std::cout << id << tweet << std::endl;
            std::stringstream t(tweet);
            while (t >> word)
            {
                wordCounts[className][word]++;
                vocab.insert(word);
            }
            totalDocuments++;
        }
    }

    std::string predict(const std::string &tweet)
    {
        double maxProb = DBL_MAX;
        std::string predictedClass;
        std::cout << vocab.size() << std::endl;

        for (const auto &entry : classCounts)
        {
            const std::string &className = entry.first;

            double classProb = log(classCounts[className] / totalDocuments);

            std::stringstream ss(tweet);
            std::string word;
            double docProb = 1;

            while (ss >> word)
            {
                // std::cout << className << " " << wordCounts[className][word] << std::endl;
                if (wordCounts[className].count(word))
                {
                    // std::cout << docProb << std::endl;
                    docProb *= abs(log((wordCounts[className][word] + 1)) - log(vocab.size()) - log(classCounts[className]));
                }
                else
                {
                    docProb *= 1;
                }
                // std::cout << wordCounts[className][word] << std::endl;
            }
            // std::cout << docProb << std::endl;
            docProb *= classProb;
            std::cout << className << classCounts[className] / totalDocuments << std::endl;
            if (docProb <= maxProb)
            {
                maxProb = docProb;
                predictedClass = className;
            }
        }
        return predictedClass;
    }
};

int main()
{
    NaiveBayesClassifier classifier;
    classifier.train("twitter_training.csv");
    std::cout << "Enter a tweet to classify" << std::endl;
    std::string input;
    getline(std::cin, input);
    std::string predictedClass = classifier.predict(input);
    std::cout << "Predicted Class : " << predictedClass << std::endl;
    return 0;
}