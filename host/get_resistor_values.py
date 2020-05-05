
def subset_sum(array_in):

    n = len(array_in)
    total_combinations = 1<<n

    for i in range(0,total_combinations):
        sub_sum = 0

        for j in range (0,n):
            if i & (1<<j):
                sub_sum += array_in[j]

        print(sub_sum)

array_in = [2000, 3900, 16400, 8200, 33000, 150000, 510000]
subset_sum(array_in)