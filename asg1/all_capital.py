all_caps = []

with open("unique_words.txt", 'r') as f:
    for line in f:
        all_caps.append(line.strip().upper())

with open("all_caps.txt", 'w') as f:
    for word in all_caps:
        f.write(f"{word}\n")