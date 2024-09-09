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