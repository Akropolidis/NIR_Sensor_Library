import csv
import yaml

NUTRIENTS = [
    'calories',
    'fat',
    'saturated_fat',
    'trans_fat',
    'carbohydrate',
    'fibre',
    'sugars',
    'protein',
    'sodium'
]

def update_nutr_dict(nutr_yaml, nutr_dict, food, mass):
    for i, nutr in enumerate(NUTRIENTS):
        nutr_dict[i] += nutr_yaml[food][nutr] * mass

def update_nutr(parsed_yaml):
    with open("food_data.csv", 'r') as infile:
        reader = csv.reader(infile)
        with open("food_data_nutr.csv", 'a', newline='') as outfile:
            writer = csv.writer(outfile)
            for line in reader:
                nutr_arr = [0]*len(NUTRIENTS) #parallel array to NUTRIENTS
                num_con = int(line[0])
                for con in range(num_con):
                    food = line[1+con*2]
                    mass = float(line[1+con*2+1])
                    update_nutr_dict(parsed_yaml, nutr_arr, food, mass)
                writer.writerow(line + nutr_arr)

def main():
    with open("nutrition.yaml") as stream:
        try:
            parsed_yaml = yaml.safe_load(stream)
            update_nutr(parsed_yaml)
        except yaml.YAMLError as exc:
            print(exc)

if __name__ == "__main__":
    main()