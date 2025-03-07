from sklearn.cluster import KMeans
import numpy as np
import matplotlib.pyplot as plt
import csv
from enum import Enum
from sklearn.preprocessing import normalize

class Food(Enum):
    CASHEW=0,
    ALMOND=1,
    BREAD=2

label_dict = {
    "cashew" : 0,
    "almond" : 1,
    "bread": 2
}

def main():
    with open("food_data.csv", 'r') as file:
        reader = csv.reader(file)
        data = [row for row in reader]
    data = np.array(data)
    labels = np.copy(data[:, 0])
    data = np.delete(data, 0, 1)
    data = np.delete(data, 0, 1)
    #print(data.shape)

    data = normalize(data, norm='l2', axis=1)

    k = 3
    kmeans = KMeans(n_clusters=k, random_state=42)
    kmeans.fit(data)

    k_labels = kmeans.labels_

    print("Truth | Guess")
    samples = data.shape[0]
    correct_total = 0
    incorrect_total = 0
    for i in range(0, len(labels)):
        print(f"{labels[i]} | {k_labels[i]}", end="")
        if label_dict[labels[i]] == k_labels[i]:
            print(" -> correct")
            correct_total +=1 
        else:
            print(" -> incorrect")
            incorrect_total += 1
    print()
    print("Stats:")
    pct_corr = correct_total/samples * 100
    print(f"correct: {correct_total} / {samples} -> {round(pct_corr, 2)} %")
    print(f"incorrect: {incorrect_total} / {samples}")
            
if __name__ == "__main__":
    main()