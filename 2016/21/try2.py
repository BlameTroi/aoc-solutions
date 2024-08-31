# day21.py
from copy import copy


def swap_positions(pos1, pos2, scrambled):
    scrambled = copy(scrambled)
    tmp = scrambled[pos1]
    scrambled[pos1] = scrambled[pos2]
    scrambled[pos2] = tmp

    return scrambled


def swap_characters(c1, c2, scrambled):
    return swap_positions(scrambled.index(c1), scrambled.index(c2), scrambled)


def reverse(start, stop, scrambled):
    reversed_part = list(reversed(scrambled[start: stop + 1]))
    return scrambled[:start] + reversed_part + scrambled[stop + 1:]


def rotate(steps, scrambled):
    left = steps < 0
    steps = abs(steps) % len(scrambled)

    if not left:
        steps = len(scrambled) - steps

    return scrambled[steps:] + scrambled[:steps]


def rotate_by_letter(letter, scrambled):
    pos = scrambled.index(letter)
    steps = pos + (1 if pos < 4 else 2)
    return rotate(steps, scrambled)


def move(pos1, pos2, scrambled):
    tmp_lst = copy(scrambled)
    del tmp_lst[pos1]
    tmp_lst.insert(pos2, scrambled[pos1])
    return tmp_lst


def unrotate_by_letter(letter, scrambled):
    pos = scrambled.index(letter)
    steps = -(pos // 2 + (1 if ((pos % 2) or not pos) else 5))
    return rotate(steps, scrambled)


def scrambler(instr, scrambled, unscrambling=False):
    instr = instr.split(' ')
    action = instr[0]

    def swap_if_unscrambling(arg1, arg2):
        if not unscrambling:
            return arg1, arg2
        else:
            return arg2, arg1

    if action == 'swap':
        if instr[1] == 'position':
            pos1, pos2 = swap_if_unscrambling(int(instr[2]), int(instr[5]))
            return swap_positions(pos1, pos2, scrambled)
        else:
            pos1, pos2 = swap_if_unscrambling(instr[2], instr[5])
            return swap_characters(pos1, pos2, scrambled)

    elif action == 'move':
        pos1, pos2 = swap_if_unscrambling(int(instr[2]), int(instr[5]))
        return move(pos1, pos2, scrambled)

    elif action == 'rotate':
        type_ = instr[1]
        if type_ in ['left', 'right']:
            flip_direction = (type_ == 'left') ^ unscrambling
            steps = int(instr[2]) * (-1 if flip_direction else 1)
            return rotate(steps, scrambled)
        else:
            oper = rotate_by_letter if not unscrambling else unrotate_by_letter
            return oper(instr[6], scrambled)

    elif action == 'reverse':  # it's own reverse
        return reverse(int(instr[2]), int(instr[4]), scrambled)


def solve(lines):
    scrambled = list('abcdefgh')
    for instr in lines:
        scrambled = scrambler(instr, scrambled)

    unscrambled = list('fbgdceah')
    for instr in reversed(lines):
        unscrambled = scrambler(instr, unscrambled, True)

    return ''.join(scrambled), ''.join(unscrambled)



instructions = []
for line in open('unitdata.txt', 'r'):
    instructions.append(line.rstrip())
print(solve(instructions))
input()
