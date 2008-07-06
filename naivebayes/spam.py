#!/usr/bin/env python
"""Naive bayes classifier for short text.

Bag of words, one multinomial per class and Laplace smoothing so that a
word never seen in a class does not kill the whole product.
"""

import re

WORD = re.compile("[a-z']+")


def tokenize(text):
    return WORD.findall(text.lower())


class NaiveBayes:

    def __init__(self):
        self.counts = {}        # class -> word -> times seen
        self.totals = {}        # class -> words seen in that class
        self.documents = {}     # class -> documents seen
        self.vocabulary = {}

    def train(self, text, label):
        if label not in self.counts:
            self.counts[label] = {}
            self.totals[label] = 0
            self.documents[label] = 0

        self.documents[label] += 1
        words = self.counts[label]

        for word in tokenize(text):
            words[word] = words.get(word, 0) + 1
            self.totals[label] += 1
            self.vocabulary[word] = True

    def word_probability(self, word, label):
        # add one smoothing over the whole vocabulary
        seen = self.counts[label].get(word, 0)
        return float(seen + 1) / (self.totals[label] + len(self.vocabulary))

    def score(self, text, label):
        total_documents = 0
        for count in self.documents.values():
            total_documents += count

        probability = float(self.documents[label]) / total_documents
        for word in tokenize(text):
            probability *= self.word_probability(word, label)
        return probability

    def classify(self, text):
        best = None
        best_score = -1.0
        for label in self.counts:
            value = self.score(text, label)
            if value > best_score:
                best_score = value
                best = label
        return best


SPAM = [
    "buy cheap pills online now",
    "cheap watches free shipping",
    "win a free prize click here",
    "make money fast working from home",
    "free free free click now",
]

HAM = [
    "are we still on for lunch tomorrow",
    "the meeting was moved to friday",
    "can you send me the report please",
    "lunch with the team on thursday",
    "here is the draft of the report",
]


def trained():
    model = NaiveBayes()
    for text in SPAM:
        model.train(text, "spam")
    for text in HAM:
        model.train(text, "ham")
    return model


if __name__ == "__main__":
    model = trained()
    for text in ["free money click here", "send me the report before friday"]:
        print("%-40s -> %s" % (text, model.classify(text)))
