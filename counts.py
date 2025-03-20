import csv

foods = {}
with open("food_data.csv", 'r') as infile:
    reader = csv.reader(infile)
    for line in reader:
        foods[str(line[1])] = foods.get(str(line[1]), 0) + 1
for key, value in foods.items():
    print(f"{key}: {value}")
