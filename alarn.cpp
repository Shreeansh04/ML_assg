#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
using namespace std;
vector<string>Earthquake;
vector<string>Alarm;
vector<string>John;
vector<string>Mary;
double calculateProbability(const std::vector<std::string>& column) {
    int trueCount = 0;
    for (const std::string& value : column) {
        if (value == "True") {
            trueCount++;
        }
    }
    return static_cast<double>(trueCount) / column.size();
}
double calculateConditionalProbability(const std::vector<std::vector<std::string>>& data, const std::string& eventA, const std::string& conditionA, const std::string& conditionB, const std::string& eventB, bool outcome) {
    int eventAColumn;
    int eventBColumn;
    
    if (eventA == "Earthquake") {
        eventAColumn = 0; // Earthquake column index (starting from 0)
    } else if (eventA == "Alarm") {
        eventAColumn = 1; // Alarm column index (starting from 0)
    } else if (eventA == "John") {
        eventAColumn = 2; // John column index (starting from 0)
    } else if (eventA == "Mary") {
        eventAColumn = 3; // Mary column index (starting from 0)
    } else {
        std::cerr << "Invalid event A name" << std::endl;
        return 0.0;
    }
    
    if (eventB == "Earthquake") {
        eventBColumn = 0; // Earthquake column index (starting from 0)
    } else if (eventB == "Alarm") {
        eventBColumn = 1; // Alarm column index (starting from 0)
    } else if (eventB == "John") {
        eventBColumn = 2; // John column index (starting from 0)
    } else if (eventB == "Mary") {
        eventBColumn = 3; // Mary column index (starting from 0)
    } else {
        std::cerr << "Invalid event B name" << std::endl;
        return 0.0;
    }

    int countConditionA = 0;
    int countConditionB = 0;
    int countBothConditions = 0;

    for (const auto& row : data) {
        if (row[eventAColumn] == conditionA) { // Assuming Alarm is the second column (index 1)
            countConditionA++;
            if ((outcome && row[eventBColumn] == "True") || (!outcome && row[eventBColumn] == "False")) {
                countBothConditions++;
            }
        }
        if (row[eventAColumn] == conditionB) { // Assuming Alarm is the second column (index 1)
            countConditionB++;
        }
    }

    if (countConditionB == 0) {
        std::cerr << "No instances found for condition B" << std::endl;
        return 0.0;
    }

    return static_cast<double>(countBothConditions) / countConditionB;
}
void ReadCSV(string file1)
{
    ifstream file;
    file.open(file1);
    string line;
   std::vector<std::vector<std::string>> data;
    
    // Read the file line by line and parse the columns
    while (std::getline(file, line)) {
        std::stringstream s(line);
       std::string item;
        std::vector<std::string> row;
        while (std::getline(s, item, ',')) {
            row.push_back(item);
        }
        data.push_back(row);
    }
    
    file.close();

    // Assuming the CSV has 4 columns: Earthquake, Alarm, John, Mary
    for (int i = 0; i < 4; ++i) {
        std::vector<std::string> column;
        for (const auto& row : data) {
            column.push_back(row[i]);
        }
        // double probability = calculateProbability(column);
        // std::cout << "Probability of 'true' in column " << i + 1 << ": " << probability << std::endl;
    }
      // Calculate conditional probabilities for John
    double johnRangGivenAlarm = calculateConditionalProbability(data, "John", "True", "True","Alarm" ,true);
    double johnDidNotRingGivenAlarm = calculateConditionalProbability(data, "John", "True", "True","Alarm", false);
    double johnRangGivenNoAlarm = calculateConditionalProbability(data, "John", "False", "False","Alarm" ,true);
    double johnDidNotRingGivenNoAlarm = calculateConditionalProbability(data, "John", "False", "False","Alarm" ,false);

    // Calculate conditional probabilities for Mary
    double maryRangGivenAlarm = calculateConditionalProbability(data, "Mary", "True", "True","Alarm" ,true);
    double maryDidNotRingGivenAlarm = calculateConditionalProbability(data, "Mary", "True", "True","Alarm",false);
    double maryRangGivenNoAlarm = calculateConditionalProbability(data, "Mary", "False", "False","Alarm",true);
    double maryDidNotRingGivenNoAlarm = calculateConditionalProbability(data, "Mary", "False", "False", "Alarm",false);
    //double alarmGivenNoEarthquake = calculateConditionalProbability(data, "Alarm", "False", "True", "Earthquake", true);

    bool userEarthquake;
    bool userAlarm;
    cout << "Was there an Earthquake? (true/false): ";
    cin >> userEarthquake;
    // cout << "Did the alarm ring? (true/false): ";
    // cin >> userAlarm;

    // Calculate probabilities using Bayes' theorem based on user inputs
    double alarmGivenEarthquake = calculateConditionalProbability(data, "Alarm", "True", "True", "Earthquake", userEarthquake);
    double johnGivenEarthquake = calculateConditionalProbability(data, "John", "True", "False", "Earthquake", userEarthquake);
    double alarmGivenNoEarthquake = calculateConditionalProbability(data, "Alarm", "True", "False", "Earthquake", false);
    double johnProbability = (johnGivenEarthquake * alarmGivenEarthquake) / alarmGivenNoEarthquake;

    // Output results
    std::cout << "Probability that the alarm rang given the Earthquake: " << alarmGivenEarthquake << std::endl;
    std::cout << "Probability that John called given the Alarm and Earthquake: " << johnProbability << std::endl;


    // double bothcalledgivenalarm = maryRangGivenAlarm*johnRangGivenAlarm*alarmGivenNoEarthquake*(1-0.579545);
    // cout <<"Prob both called" << bothcalledgivenalarm << endl;
}
int main() {
    ReadCSV("Alarm.csv");
}