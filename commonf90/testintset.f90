program testintset

   use iso_fortran_env, only: int64
   use intset, only: size_of_set, clear_set, include_in_set, exclude_from_set, intersection_of_sets, &
                     union_of_sets, intset_limit, contains_set, is_in_set

   implicit none

   integer  :: i                   ! work
   logical  :: one(0:intset_limit), two(0:intset_limit), onetwo(0:intset_limit)
   integer  :: onelo, onehi, twolo, twohi ! range endpoints

   onelo = 12
   onehi = 17
   twolo = 13
   twohi = 15

   call clear_set(one)
   call clear_set(two)
   call clear_set(onetwo)

   do i = onelo, onehi
      call include_in_set(one, i)
   end do

   do i = twolo, twohi
      call include_in_set(two, i)
   end do

   if (contains_set(one, two)) then
      print *, "one contains two"
   end if

   if (contains_set(two, one)) then
      print *, "two contains one"
   end if

   if (is_in_set(onelo, one)) then
      print *, "one correctly contains", onelo
   end if

   if (is_in_set(onelo - 1, one)) then
      print *, "one incorrectly contains", onelo - 1
   end if

end program testintset
