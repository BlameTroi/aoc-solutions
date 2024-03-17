program splitter
   implicit none

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
      character(len=:), allocatable :: words(:)
      integer                       :: i
      words = split_str(str, sep)
      write (*, *) "string     = [", trim(str), "]"
      write (*, *) "separators = [", trim(sep), "]"
      do i = lbound(words, 1), ubound(words, 1)
         write (*, "('word: ', i2, ' len: ', i3, ' contents: [', a, ']')") i, len(words(i)), words(i)
      end do
      write (*, *)
   end subroutine test_split_str

   ! split a longer string into tokens at separators. all separators consumed
   ! with no meaningful error handling. it shouldn't crash, but garbage in will
   ! get garbage out.
   !
   ! bugs:
   !
   ! - fortran doesn't have zero length strings in the sense most people think
   !   of them, so some of the edge cases return one single blank.

   function split_str(str, sep) result(res)
      implicit none
      character(len=*), intent(in) :: str
      character(len=*), intent(in) :: sep
      character(len=:), allocatable :: res(:)
      integer :: i, j, k
      logical :: in_word
      integer :: longest, count

      ! guard against empty string
      if (len_trim(str) == 0) then
         allocate (character(len=1) :: res(1))
         res(1) = ""
         return
      end if

      ! guard against empty separator, distinct from a single space
      if (len(sep) == 0) then
         allocate (character(len=len(str)) :: res(1))
         res(1) = str
         return
      end if

      ! count actual number and find longest
      longest = 0
      count = 0
      in_word = .false.
      j = 0
      do i = 1, len(str)
         if (index(sep, str(i:i)) /= 0) then
            if (in_word) then
               count = count + 1
               longest = max(longest, j)
               j = 0
               in_word = .false.
            end if
         else
            j = j + 1
            in_word = .true.
         end if
      end do
      ! don't forget the last word
      if (in_word) then
         count = count + 1
         longest = max(longest, j)
      end if

      ! handle no words found, single entry empty string
      if (count == 0) then
         allocate (character(len=1) :: res(1))
         res(1) = ""
         return
      end if

      ! copy into result
      allocate (character(len=longest) :: res(count))
      in_word = .false.
      j = 1
      k = 1
      res(j) = repeat(" ", longest)
      do i = 1, len(str)
         if (index(sep, str(i:i)) /= 0) then
            if (in_word) then
               k = 1
               j = j + 1
               res(j) = ""
               in_word = .false.
            end if
         else
            res(j) (k:k) = str(i:i)
            k = k + 1
            in_word = .true.
         end if
      end do

   end function split_str

end program splitter
