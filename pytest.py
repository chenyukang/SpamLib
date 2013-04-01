#!/usr/bin/env python
#coding=utf-8
import os
import sys
import random
import unittest
import ac_dict

conf_path = "sensitive_words.conf"

class T(unittest.TestCase):

    def all_key_word(self): #libWordFilter default use conf_path initialize
        self.assertEqual(os.path.isfile(conf_path), True)
        d = ac_dict.AC_Dict(conf_path)
        self.assertNotEqual(d, None)
        f = open(conf_path, 'r')
        lines = f.readlines()
        for l in lines:
            l = l[:-1]
            res = d.check(l)
            self.assertEqual(res, '*' * len(res))
        f.close()

    def dict_reset(self):
        self.assertEqual(os.path.isfile(conf_path), True)
        d = ac_dict.AC_Dict("no")
        self.assertEqual(d, None)
        d = ac_dict.AC_Dict(conf_path)
        self.assertEqual(d.check("毛泽东"), "***")
        self.assertEqual(d.check("邓小平"), "***")

    def dictory_all_words_test(self):
        f = "/usr/share/dict/words"
        fp = open(f)
        out = "words"
        outp = open(out, "w")
        for i, word in enumerate(fp):
            w = word.strip()
            s = ""
            for x in w:
                if (x>='a' and x<='z') or \
                   (x>='A' and x<='Z') or \
                   x == '\'':
                    s += x
            outp.write(s + "\n")
        fp.close()
        outp.close()

        d = ac_dict.AC_Dict(out)
        fp = open(out)
        for i, word in enumerate(fp):
            s = word.strip()
            res = d.check(s)
            self.assertEqual(res, '*'*len(res))
        os.system("rm words")
        
    def random_test(self):
        str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        f = open(conf_path, "r")
        lines = f.readlines()
        f.close()
        d = ac_dict.AC_Dict(conf_path)
        for k in range(0, 10000):
            random_len = random.randint(2, 200)
            s = ""
            for k in range(0, random_len):
                s += str[random.randint(0, len(str)-1)]
            res = d.check(s)
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
        d = ac_dict.AC_Dict(conf_path);
        for k in range(0, 10000):
            random_len = random.randint(2, 200)
            s = ""
            for k in range(0, random_len):
                s += str[random.randint(0, len(str)-1)]
            res = d.check(s)
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
    suite.addTest(T("dictory_all_words_test"))
    suite.addTest(T("random_test"))
    suite.addTest(T("random_test_two"))
    return suite


if __name__ == "__main__":
    unittest.TextTestRunner().run(suite())
