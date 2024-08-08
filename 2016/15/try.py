# t = 0
# while True:
#     t += 1
#     if (t+15+1)%17 == 0 and (t+2+2)%3 == 0 and (t+4+3)%19 == 0 and (t+2+4)%13 == 0 and (t+2+5)%7 == 0 and (t+6)%5 == 0:
#         print (t)
#         break
# Disc #1 has 13 positions; at time=0, it is at position 10.   drop when at 12
# Disc #2 has 17 positions; at time=0, it is at position 15.   and when at 16 - 1
# Disc #3 has 19 positions; at time=0, it is at position 17.   and when at 18 - 2
# Disc #4 has 7 positions; at time=0, it is at position 1.     and when at 6  - 3
# Disc #5 has 5 positions; at time=0, it is at position 0.     and when at 4  - 4
# Disc #6 has 3 positions; at time=0, it is at position 1.     and when at 2  - 5

sizes = [13, 17, 19, 7, 5, 3]
positions = [18, 15, 17, 1, 0, 1]
targetPositions = [-i%sizes[i] for i in range(len(sizes))]
print(sizes)
print(positions)
print(targetPositions)
time = 0
while True:
    if [(positions[i]+time)%sizes[i] for i in range(len(positions))] == targetPositions:
        print(time-1)
        break
    time += 1
