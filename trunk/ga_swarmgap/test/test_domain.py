import unittest
import domain

class TestDiscreteDomain(unittest.TestCase):
    def test_basic_calls(self):
        values = [2,3,4,5]
        d = domain.DiscreteDomain(values)

        self.assertTrue(d.has_value(2))
        self.assertFalse(d.has_value(2.5))
        self.assertTrue(d.has_value(4))

        self.assertEqual(2, d.min_value())
        self.assertEqual(5, d.max_value())

        for i in range(0, 100):
            self.assertTrue(d.random_value() in values)

    def test_standard_interval(self):
        d = domain.DiscreteDomain(domain.STANDARD_INTERVAL)
        self.assertEqual(0, d.min_value())
        self.assertEqual(1, d.max_value())

        self.assertFalse(d.has_value(0.17))
        self.assertTrue(d.has_value(.25))

        for i in range(0, 100):
            self.assertTrue(d.random_value() in domain.STANDARD_INTERVAL)

if __name__ == '__main__':
    unittest.main()