! testhelpers.f90 -- quick tests for aochelpers.f90 -- troy brumley blametroi@gmail.com

program testhelpers
   use iso_fortran_env, only: int64
   use aochelpers

   implicit none

   integer                       :: i, j, k;
   integer(kind=int64)           :: xi, xj, xk
   character(len=:), allocatable :: s
   integer                       :: p

   print *, "is_digit('') ", is_digit("")
   print *, "is_digit('0one')", is_digit('0one')
   print *, "char_int('a') =", char_int("a")
   print *, "char_int('0') =", char_int("0")
   print *, "digit_int('5') =", digit_int("5")
   print *, "digit_int('x') =", digit_int("x")
   print *, "int_digit(10) = ", int_digit(10)
   print *, "int_digit(3) = ", int_digit(3)
   print *, "int_digit(-3) = ", int_digit(-3)
   xi = 12345
   xj = -543210
   xk = 0
   print *, "xi = 12345; int64_str(xi) =", int64_str(xi)
   print *, "xj = -543210; int64_str(xi) =", int64_str(xj)
   print *, "xk = 0; int64_str(xi) =", int64_str(xk)
   xi = huge(xi)
   s = int64_str(xi)
   print *, "xi = huge(xi); int64_str(xi) =", int64_str(xi)
   xi = 27123
   if (int64_str(xi) /= "27123") print *, "int64_str(27123) fail got ", int64_str(xi)

   i = 12345
   j = -543210
   k = 0
   print *, "i = 12345; int_str(i) =", int_str(i)
   print *, "j = -543210; int_str(i) =", int_str(j)
   print *, "k = 0; int_str(i) =", int_str(k)
   i = huge(i)
   s = int_str(i)
   print *, "i = huge(i); int_str(i) =", int_str(i)
   i = 271235
   if (int_str(i) /= "27123") print *, "int_str(27123) fail got ", int_str(i)

   print *, "is_digit('a') ", is_digit("a")
   print *, "is_digit('9') ", is_digit("9")
   print *, "is_letter('z') ", is_letter("z")
   print *, "is_letter('?') ", is_letter("?")

   print *, "is_char_in('a', 'asdf') ", is_char_in("a", "asdf")
   print *, "is_char_in('?', 'question?') ", is_char_in("?", "question?")
   print *, "is_char_in('A', 'alphabet') ", is_char_in("A", "alphabet")
   print *, "is_char_in('', '') ", is_char_in("", "")

   p = 1
   print *, "p = 1; str_int('3one', p) =", str_int("3one", p), "p=", p
   p = 2
   print *, "p = 2; str_int('3123', p) =", str_int("3123", p), "p=", p
end program testhelpers
