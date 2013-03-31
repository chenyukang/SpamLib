#coding=utf-8
import os
import sys
import random
import unittest
import libWordFilter

conf_path = "sensitive_words.conf"

def check_str(str):
    return libWordFilter.check(str)

class T(unittest.TestCase):

    def all_key_word(self): #libWordFilter default use conf_path initialize
        self.assertEqual(os.path.isfile(conf_path), True)
        f = open(conf_path, 'r')
        lines = f.readlines()
        for l in lines:
            l = l[:-1]
            res = libWordFilter.check(l)
            self.assertEqual(res, '*' * len(res))
        f.close()

    def dict_reset(self):
        self.assertEqual(os.path.isfile(conf_path), True)
        res = libWordFilter.init("no")
        self.assertEqual(res, None)
        #now libWordFilter is in error
        self.assertEqual(libWordFilter.check("毛泽东"), "***")
        libWordFilter.init(conf_path)
        self.assertEqual(libWordFilter.check("毛泽东"), "***")
        libWordFilter.init(conf_path)

    def random_test(self):
        str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        f = open(conf_path, "r")
        lines = f.readlines()
        f.close()
        for k in range(0, 10000):
            random_len = random.randint(2, 200)
            s = ""
            for k in range(0, random_len):
                s += str[random.randint(0, len(str)-1)]
            res = libWordFilter.check(s)
            if res != s:
                found = False;
                for l in lines:
                    if s.find(l[0:-1]) != -1:
                        found = True
                        break
                self.assertEqual(found, True)

    def random_test_two(self):
        str = "0123456789~!@#$%^&*()_+{}|:\"<>?abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        f = open(conf_path, "r")
        lines = f.readlines()
        f.close()
        for k in range(0, 10000):
            random_len = random.randint(2, 200)
            s = ""
            for k in range(0, random_len):
                s += str[random.randint(0, len(str)-1)]
            res = libWordFilter.check(s)
            if res != s:
                found = False;
                for l in lines:
                    if s.find(l[0:-1]) != -1:
                        found = True
                        break
                self.assertEqual(found, True)


def suite():
    suite = unittest.TestSuite()
    suite.addTest(T("all_key_word"))
    suite.addTest(T("dict_reset"))
    suite.addTest(T("random_test"))
    suite.addTest(T("random_test_two"))
    return suite


if __name__ == "__main__":
    unittest.TextTestRunner().run(suite())
