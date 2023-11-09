#include <bits/stdc++.h>
#include <cmath>

struct Node
{
    std::string name;
    std::vector<std::string> parents;
    std::vector<double> probabilities;
};

class BayesianNetwork
{
private:
    std::vector<Node> nodes;

public:
    void addNode(const Node &node)
    {
        nodes.push_back(node);
    }

    double calculateProbability(const Node &node, const std::vector<bool> &parentStates, bool currentState)
    {
        int index = 0;
        int parentIndex = 0;
        for (int j = 0; j < node.parents.size(); j++)
        {
            parentIndex += parentStates[j] ? pow(2, j) : 0;
        }
        return currentState ? node.probabilities[parentIndex] : 1 - node.probabilities[parentIndex];
    }

    double calculateProbability(const Node &node, const std::vector<bool> &evidence)
    {
        double probability = 1.0;
        for (int i = 0; i < node.parents.size(); i++)
        {
            double parentProbability = calculateProbability(nodes[i], evidence, evidence[i]);
            probability *= parentProbability;
        }
        return probability;
    }
};

int main()
{
    Node Asian = {"Asian", {}, {0.01}};
    Node Tuberculosis = {"Tuberculosis", {"Asian"}, {0.01, 0.05}};
    Node Smoke = {"Smoke", {}, {0.5}};
    Node Lung = {"Lung", {"Smoke"}, {0.01, 0.1}};
    Node Either = {"Either", {"Tuberculosis", "Lung"}, {1.0, 1.0, 1.0, 0.0}};
    Node Bronc = {"Bronc", {"Smoke"}, {0.6}};
    Node Dyspnea = {"Dyspnea", {"Either", "Bronc"}, {0.9, 0.8, 0.7, 0.1}};
    Node Xray = {"Xray", {"Either"}, {0.98}};

    BayesianNetwork net;
    net.addNode(Asian);
    net.addNode(Tuberculosis);
    net.addNode(Smoke);
    net.addNode(Lung);
    net.addNode(Either);
    net.addNode(Bronc);
    net.addNode(Dyspnea);
    net.addNode(Xray);

    std::vector<bool> evidence = {true, false, true, false, true, false, false, true}; // asian who doesn't smoke but has positive Xray
    double prob_xray = net.calculateProbability(Xray, evidence);
    std::cout << "P(Xray | Asian,!Smoke) " << prob_xray << std::endl;

    return 0;
}