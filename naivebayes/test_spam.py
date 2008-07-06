#!/usr/bin/env python
"""Tests for the naive bayes classifier."""

import unittest

import spam


class TokenizeTest(unittest.TestCase):

    def test_lowercases_and_splits(self):
        self.assertEqual(["hello", "world"], spam.tokenize("Hello, World!"))

    def test_keeps_apostrophes(self):
        self.assertEqual(["don't", "go"], spam.tokenize("don't go"))

    def test_drops_numbers(self):
        self.assertEqual(["room"], spam.tokenize("room 101"))


class ClassifyTest(unittest.TestCase):

    def setUp(self):
        self.model = spam.trained()

    def test_obvious_spam(self):
        self.assertEqual("spam", self.model.classify("free money click here now"))

    def test_obvious_ham(self):
        self.assertEqual("ham", self.model.classify("can you send the report"))

    def test_training_documents_are_classified_right(self):
        for text in spam.SPAM:
            self.assertEqual("spam", self.model.classify(text))
        for text in spam.HAM:
            self.assertEqual("ham", self.model.classify(text))

    def test_unknown_words_do_not_break_it(self):
        # smoothing keeps the score above zero even for a word never seen
        self.assertTrue(self.model.score("zurbaran", "spam") > 0.0)
        self.assertTrue(self.model.classify("zurbaran") is not None)


if __name__ == "__main__":
    unittest.main()
