# import sys

# args = sys.argv[1:]
# text = []
# for i in range(len(args)):
#     with open(args[i], 'r') as f:
#         for line in f:
#             # words.extend(line.split())
#             text.append(line)


# # remove punctuation
# words = [word.strip('\'-+=_;:.,!?()[]{}"') for word in words]

# # remove empty strings
# words = [word for word in words if word]

# # convert to lowercase
# words = [word.lower() for word in words]

# # filter out unique words
# words = list(set(words))
# words.sort()

# # sort by frequency
# # words.sort(key=lambda x: words.count(x), reverse=True)

import sys
import string
from collections import Counter


def process_book(file_path):
    with open(file_path, 'r') as f:
        text = f.read()

    # Remove punctuation
    translator = str.maketrans('', '', string.punctuation)
    text = text.translate(translator)

    # Convert to lowercase and split into words
    words = text.lower().split()

    # Count the frequency of each word
    word_counts = Counter(words)

    # Sort words by frequency (highest first) and then alphabetically
    sorted_words = sorted(word_counts.items(), key=lambda x: (-x[1], x[0]))

    # Print words and their frequencies
    for word, count in sorted_words:
        print(f"{word}: {count}")

    counter = 0
    with open('unique_words.txt', 'w') as f:
        for word, count in sorted_words:
            f.write(f"{word}\n")
            counter += 1
            # f.write(f"{word}: {count}\n")
    
    print(f"Number of unique words: {counter}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python process_book.py book.txt")
        sys.exit(1)

    for i in range(1, len(sys.argv)):
        book_file = sys.argv[i]
        process_book(book_file)

# print(words)
# print(len(words))

