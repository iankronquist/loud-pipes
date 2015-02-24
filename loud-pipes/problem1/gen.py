
def gen_odds():
    a = 1
    while True:
        a += 2
        yield a


odd_generator = gen_odds()

for i in range(10):
    print next(odd_generator)
