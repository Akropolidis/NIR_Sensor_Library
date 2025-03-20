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

def nutri_facts(yaml):
    plate_mass = float(input("Mass of plate: "))
    num_con = int(input("Number of constituents:"))
    con_arr = []
    total_mass = plate_mass
    for i in range(num_con):
        sample_mass = float(input("Mass of sample: ")) - total_mass
        food = input("Food being scanned: ")
        con_arr.append([food, sample_mass])
        total_mass += sample_mass
    nutr_arr = [0]*len(NUTRIENTS) #parallel array to NUTRIENTS
    for pair in con_arr:
        for i, nutr in enumerate(NUTRIENTS): 
            nutr_arr[i] += yaml[pair[0]][nutr] * pair[1]
    for i, n in enumerate(NUTRIENTS):
        print(f"{n}: {nutr_arr[i]:.2f} g")

def main():
    with open("nutrition.yaml") as stream:
        try:
            parsed_yaml = yaml.safe_load(stream)
            nutri_facts(parsed_yaml)
        except yaml.YAMLError as exc:
            print(exc)

if __name__ == "__main__":
    main()