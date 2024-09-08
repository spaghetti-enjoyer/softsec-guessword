import os
import glob

# Specify the directory containing the text files
directory = "preprocessed/dictionary/gutenberg/"

# Get a list of all text files in the directory
file_list = glob.glob(os.path.join(directory, "*.txt"))

words = []

# Read and process each file
for file_path in file_list:
    with open(file_path, 'r') as f:
        file_words = f.read().strip().split()
        words.extend(file_words)

# Convert all words to lowercase
words = [word.lower() for word in words]

# Remove duplicates by converting the list to a set and back to a list
unique_words = list(set(words))

# remove accidental concatenations
# counter = 0
# for word in words:
#     for another_word in words:
#         if word in another_word and word != another_word:
#             print(word, another_word)
#             unique_words.remove(another_word)
#             counter += 1

# print("total removed:", counter)

# Write the unique words to a file

with open('unique_words.txt', 'w') as f:
    for word in unique_words:
        f.write(f"{word}\n")

# print(unique_words)
# print(len(unique_words))


# all caps
all_caps = []

with open("unique_words.txt", 'r') as f:
    for line in f:
        all_caps.append(line.strip().upper())

with open("all_caps.txt", 'w') as f:
    for word in all_caps:
        f.write(f"{word}\n")


# 1 capital
one_capital = []

with open("unique_words.txt", 'r') as f:
    for line in f:
        one_capital.append(line.strip())
        # append all combinations where 1 letter is capitalized
        for i in range(len(line) - 1):
            one_capital.append(line[:i] + line[i].upper() + line[i+1:])

with open("one_capital.txt", 'w') as f:
    for word in one_capital:
        # remove accidental \n at the end of the word
        word = word.strip()
        f.write(f"{word}\n")


# 2 word combinations
two_word_combinations = []

with open("unique_words.txt", "r") as f:
    lines = f.readlines()

counter = 0
for line in lines:
    length = len(line.strip())
    if length >= 10:
        for another_line in lines:
            if len(another_line.strip()) + length == 24:
                two_word_combinations.append(line.strip() + another_line.strip())
                counter += 1
    if counter % 100 == 0:
        print(counter)

with open("two_word_combinations.txt", 'w') as f:
    for word in two_word_combinations:
        f.write(f"{word}\n")


# funny letters

funny_letters = []
with open("unique_words.txt", "r") as f:
    lines = f.readlines()

for line in lines:
    word = line.strip()
    if 'w' in word:
        funny_letters.append(word.replace('w', '\/\/'))

    elif 't' in word:
        funny_letters.append(word.replace('t', '-|-'))

    elif 'h' in word:
        funny_letters.append(word.replace('h', '}{'))

with open("funny_letters.txt", 'w') as f:
    for word in funny_letters:
        f.write(f"{word}\n")