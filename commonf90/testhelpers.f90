! testhelpers.f90 -- quick tests for aochelpers.f90 -- troy brumley blametroi@gmail.com

program testhelpers
   use iso_fortran_env, only: int64
   use aochelpers
   ! use aochelpers, only: is_digit, char_int, digit_int, int_digit, int64_str, &
   !                       int_str, str_int, str_int64, str_ints, split_str, &
   !                       is_letter, is_char_in, sanatize_int

   implicit none

   integer                       :: i, j, k;
   integer(kind=int64)           :: xi, xj, xk
   character(len=:), allocatable :: s
   integer                       :: p

   write (*, *) "is_digit('') ", is_digit("")
   write (*, *) "is_digit('0one')", is_digit('0one')
   write (*, *) "char_int('a') =", char_int("a")
   write (*, *) "char_int('0') =", char_int("0")
   write (*, *) "digit_int('5') =", digit_int("5")
   write (*, *) "digit_int('x') =", digit_int("x")
   write (*, *) "int_digit(10) = ", int_digit(10)
   write (*, *) "int_digit(3) = ", int_digit(3)
   write (*, *) "int_digit(-3) = ", int_digit(-3)
   xi = 12345
   xj = -543210
   xk = 0
   write (*, *) "xi = 12345; int64_str(xi) =", int64_str(xi)
   write (*, *) "xj = -543210; int64_str(xi) =", int64_str(xj)
   write (*, *) "xk = 0; int64_str(xi) =", int64_str(xk)
   xi = huge(xi)
   s = int64_str(xi)
   write (*, *) "xi = huge(xi); int64_str(xi) =", int64_str(xi)
   xi = 27123
   if (int64_str(xi) /= "27123") write (*, *) "int64_str(27123) fail got ", int64_str(xi)

   i = 12345
   j = -543210
   k = 0
   write (*, *) "i = 12345; int_str(i) =", int_str(i)
   write (*, *) "j = -543210; int_str(i) =", int_str(j)
   write (*, *) "k = 0; int_str(i) =", int_str(k)
   i = huge(i)
   s = int_str(i)
   write (*, *) "i = huge(i); int_str(i) =", int_str(i)
   i = 271235
   if (int_str(i) /= "27123") write (*, *) "int_str(27123) fail got ", int_str(i)

   write (*, *) "is_digit('a') ", is_digit("a")
   write (*, *) "is_digit('9') ", is_digit("9")
   write (*, *) "is_letter('z') ", is_letter("z")
   write (*, *) "is_letter('?') ", is_letter("?")

   write (*, *) "is_char_in('a', 'asdf') ", is_char_in("a", "asdf")
   write (*, *) "is_char_in('?', 'question?') ", is_char_in("?", "question?")
   write (*, *) "is_char_in('A', 'alphabet') ", is_char_in("A", "alphabet")
   write (*, *) "is_char_in('', '') ", is_char_in("", "")

   p = 1
   write (*, *) "p = 1; str_int('3one', p) =", str_int("3one", p), "p=", p
   p = 2
   write (*, *) "p = 2; str_int('3123', p) =", str_int("3123", p), "p=", p

   write (*, *) "sanatize_int('123.45 -17 && fred=90')", sanatize_int("123.45 -17 && fred=90")

   write (*, *) "testing split_str"
   write (*, *)
   write (*, *) "some input for 2022 16"
   call test_split_str("Valve FF has flow rate=0; tunnels lead to valves EE, GG", " ;,=")
   call test_split_str("Valve GG has flow rate=0; tunnels lead to valves FF, HH", " ;,=")
   call test_split_str("Valve HH has flow rate=22; tunnel leads to valve GG", " ;,=")

   write (*, *)
   write (*, *) "edge cases"
   call test_split_str("onelongword", " ")
   call test_split_str("some text but no separator specified", "")
   call test_split_str("separator not matched", ".")
   call test_split_str("", "empty string")
   call test_split_str("all separators", "al seprto")

   write (*, *)
   write (*, *) "end test split_str"
   stop

contains

   ! test driver
   subroutine test_split_str(str, sep)
      implicit none
      character(len=*), intent(in)  :: str
      character(len=*), intent(in)  :: sep
      character(len=:), dimension(:), allocatable :: words
      integer                       :: w
      words = split_str(str, sep)
      write (*, *) "string     = [", trim(str), "]"
      write (*, *) "separators = [", trim(sep), "]"
      do w = lbound(words, 1), ubound(words, 1)
         write (*, "('word: ', i2, ' len: ', i3, ' contents: [', a, ']')") w, len(words(w)), words(w)
      end do
      write (*, *)
   end subroutine test_split_str

end program testhelpers
