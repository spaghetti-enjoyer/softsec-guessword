with open("common_names.txt", 'r') as f:
    common_names = f.read().split()

with open("names.txt", 'w') as f:
    for name in common_names:
        name = name.lower()
        f.write(f"{name}\n")