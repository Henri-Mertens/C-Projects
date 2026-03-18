def sum_positives(numbers):
    total = 0
    for n in numbers:
        if n >= 0:
            total += n
    return total

print(sum_positives([1, -2, 3, 0, -5, 4]))