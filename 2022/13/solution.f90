! working theory ... data read isn't being stored.
! solution.f90 -- 2022 day 13 -- distress signal -- troy brumley blametroi@gmail.com

! advent of code 2022 day 13 -- distress signal
!
! for part one, identify communication packets that are correctly ordered. the
! packets are lists and this looks rather lispish in lean, but we'll go with
! another language.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec
   integer, parameter            :: MAXPAIRS = 500      ! observed

   ! the only way i've been able to get an algorithm that doesn't offend my
   ! sensibilities is to get closer and closer to a lisp list. i guess they knew
   ! what they were doing way back then. a list is at least two nodes. the last
   ! node is always empty. as the problem for advent of code is dealing with lists
   ! of integers, the value payload is just an integer.
   !
   ! pointers in fortran are a bit different from pascal or c. i think they are
   ! better semantically, more like a base register for a dsect from my assembly
   ! language days, but my muscle memory with pascal and c is sometimes causing
   ! errors. worth the struggle, but still a struggle.

   ! an atomic item, an integer for this problem.
   type atom_t
      logical               :: isnull
      integer               :: val
   end type atom_t

   ! a list of lists or atoms
   type list_t
      type(atom_t), pointer :: atom
      type(list_t), pointer :: nested
      type(list_t), pointer :: next
   end type list_t

   ! input is multiple paired lists. this stores the parsed values of
   ! each pair and notes if they are in proper order.

   type pair_t
      type(list_t), pointer :: left, right
      logical :: inorder
   end type pair_t

   ! the pairs are for part one, the packets are for part two. note that the
   ! vectors here contain structures with pointers, they are not pointers as
   ! they were in the original pascal version.

   type(pair_t)            :: pair(1:MAXPAIRS)
   type(list_t), target    :: packet(1:MAXPAIRS * 2)
   type(list_t), pointer   :: templist

   integer                 :: numpairs, numpackets, curr, pk1, pk2

   ! open and initialize.

   rec = ""
   call open_aoc_input(AOCIN)
   part_one = 0; part_two = 0
   call init

   ! load pairs and seed the packets, but packets will need sorting for
   ! part two.

   call load

   ! for part one, sum the indices of the in-order pairs

   do curr = 1, numpairs
      if (pair(curr) % inorder) part_one = part_one + curr
   end do

   ! for part two, insert two missing 'divider' packets, sort them with the
   ! other packets, and report the product of their indices as a key to the
   ! solution.

   templist => strlist("[[2]]")
   packet(numpackets + 1) = templist
   templist => strlist("[[6]]")
   packet(numpackets + 2) = templist
   numpackets = numpackets + 2

   call sort

   pk1 = indexof(strlist("[[2]]"))
   pk2 = indexof(strlist("[[6]]"))
   part_two = pk1 * pk2

   ! report and close

   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   ! make sure all pointers are unassociated.

   subroutine init
      implicit none
      integer :: i

      do i = 1, MAXPAIRS

         nullify (pair(i) % left)

         nullify (pair(i) % right)

         nullify (packet(i * 2 - 1) % atom)
         nullify (packet(i * 2 - 1) % nested)
         nullify (packet(i * 2 - 1) % next)

         nullify (packet(i * 2) % atom)
         nullify (packet(i * 2) % nested)
         nullify (packet(i * 2) % next)

      end do

      numpairs = 0
      numpackets = 0
   end subroutine init

   ! read and parse into two arrays for parts one and two. part two wants the
   ! lists sorted, so since we're already doing a compare to check for pair
   ! ordering, lay each pair into the second array in order to (hopefully)
   ! save a few compare and swaps later on.

   subroutine load
      implicit none
      integer :: i, j
      character(len=max_aoc_reclen) :: s, t

      call rewind_aoc_input(AOCIN)

      ! input is at least two lines, with a third blank separator line if more
      ! pairs follow. i'm being needlessly cute here and allocating "just big
      ! enough" buffers to hold each line of input as i read them just to get
      ! comfortable with the fortran way of doing allocations. it's not really
      ! needed.
      !
      ! pairs subscript from one and advance by one (i), while packets subscript
      ! from one and advance by two (j) for each logical input.

      i = 0; j = -1

      readloop: do while (read_aoc_input(AOCIN, rec))
         i = i + 1; j = j + 2

         ! copy left to buffer. i know i could keep track of the size myself and
         ! deallocate/reallocate only when the incoming record is longer than the
         ! current buffer, but i wonder if there's a way to interrogate the size
         ! of the allocated area.

         s = rec(1:len_trim(rec))

         ! read right and copy to another buffer.

         if (.not. read_aoc_input(AOCIN, rec)) then
            print *, "*** error *** unexpected end of file"
            stop 1
         end if
         t = rec(1:len_trim(rec))

         ! advance past separator line. an end of file here is ok and will
         ! be caught by the while test above.

         if (read_aoc_input(AOCIN, rec)) then
            if (len_trim(rec) /= 0) then
               print *, "*** error *** error in input"
               stop 1
            end if
         end if

         ! parse the pair

         pair(i) % left => strlist(s)
         pair(i) % right => strlist(t)

         ! while we're here, note if they are in order for part one

         pair(i) % inorder = listcompare(pair(i) % left, pair(i) % right) < 1

         ! copy to packets for part two, order them on the way in even though it
         ! probably won't help the later sort much note: this copies the data of
         ! the root list_t, not a pointer to it, from the pair set. the anchor
         ! arrays come with the head of each list already there, and then we
         ! chain off them. as there are no backreferences, this copy shouldn't
         ! cause any problems.

         if (pair(i) % inorder) then
            packet(j) = pair(i) % left
            packet(j + 1) = pair(i) % right
         else
            packet(j) = pair(i) % right
            packet(j + 1) = pair(i) % left
         end if

      end do readloop

      numpairs = i
      numpackets = i * 2

   end subroutine load

   ! gfortran print *,int produces a lot of leading blanks. lfortran does not.
   ! intstr gives the integer as a string with only one leading and one
   ! trailing blank.

   function intstr(n) result(res)
      implicit none
      integer, intent(in)           :: n
      character(len=:), allocatable :: res
      character(len=32)             :: buf

      buf = ""
      write (buf, *) n
      allocate (character(len=len_trim(adjustl(buf))) :: res)
      res = trim(adjustl(buf))
   end function intstr

   ! fortran's displayable logical is T or F. let's get wordy.

   ! function boolstr(b) result(res)
   !    implicit none
   !    logical, intent(in)           :: b
   !    character(len=:), allocatable :: res
   !    if (b) then
   !       allocate (character(len=4) :: res)
   !       res = "True"
   !    else
   !       allocate (character(len=5) :: res)
   !       res = "False"
   !    end if
   ! end function boolstr

   ! compare two lists using the rules for part one and return via the negative
   ! zero positive convention.
   !
   ! compare the lists item by item, with the following behaviors for nested
   ! lists:
   !
   ! * a longer list comes after an otherwise equivalent list
   ! * if one item is an integer and the other is a list, promote the integer
   !   to a list and then compare as above.
   !
   ! once you know an item is out of order, you are done.

   recursive function listcompare(xin, yin) result(res)
      implicit none
      type(list_t), pointer, intent(in) :: xin, yin
      integer                           :: res
      integer                           :: d
      type(list_t), pointer             :: xs, ys, tx, ty

      ! assume they are out of order
      res = +huge(d)

      ! we need to chase through the links, so get local
      ! updatable copies of the list anchors
      xs => xin
      ys => yin

      ! item/position by item/position

      compareloop: do while (notnilorempty(xs) .and. notnilorempty(ys))

         ! easy case one, two atoms.

         if (hasvalue(xs) .and. hasvalue(ys)) then
            d = getvalue(xs) - getvalue(ys)
            if (d /= 0) then
               res = d
               return
            end if
            xs => xs % next; ys => ys % next
            cycle compareloop
         end if

         ! easy case two, two nested lists.

         if (hasnested(xs) .and. hasnested(ys)) then
            d = listcompare(getnested(xs), getnested(ys))
            if (d /= 0) then
               res = d
               return
            end if
            xs => xs % next; ys => ys % next
            cycle compareloop
         end if

         ! less easy case one, atom to list. here we cast the atom to a list and
         ! compare it to the other list, being sure to free the temporary list
         ! when the compare completes.

         if (hasvalue(xs) .and. hasnested(ys)) then
            tx => listfromvalue(xs)
            d = listcompare(tx, getnested(ys))
            call freelist(tx)
            if (d /= 0) then
               res = d
               return
            end if
            xs => xs % next; ys => ys % next
            cycle compareloop
         end if

         ! same song, other foot.

         if (hasnested(xs) .and. hasvalue(ys)) then
            ty => listfromvalue(ys)
            d = listcompare(getnested(xs), ty)
            call freelist(ty)
            if (d /= 0) then
               res = d
               return
            end if
            xs => xs % next; ys => ys % next
            cycle compareloop
         end if

         ! should not occur

         print *, "*** error *** invalid list structure, aborting in listcompare"
         stop 1

      end do compareloop

      ! if both lists are exhausted, equal. if left empty, in order. if right
      ! empty, out of order.

      if (isnilorempty(xs) .and. isnilorempty(ys)) then
         res = 0
      else if (isnilorempty(xs)) then
         res = -huge(d)
      else
         res = huge(d)
      end if
   end function listcompare

   ! list node accessors and predicates. i prefer an api instead of expposing
   ! the structure/link paths in higher level code.

   ! a list can contain another list. xs is marked inout, but we only update
   ! its members, not its value.

   subroutine setnested(xs, ys)
      implicit none
      type(list_t), pointer, intent(inout) :: xs
      type(list_t), pointer, intent(in) :: ys

      if (associated(xs % atom)) then
         print *, "*** error *** assigning nested to an atomic node"
         stop 1
      end if
      xs % nested => ys
   end subroutine setnested

   function getnested(xs) result(res)
      implicit none
      type(list_t), pointer :: res
      type(list_t), pointer, intent(in) :: xs

      res => xs % nested
   end function getnested

   ! a list can contain an atomic value, in this application, an integer. the
   ! value could be stored directly, but if this where a real application and
   ! more atomic types were added, it would make more sense and be a better use
   ! of storage to keep the list nodes as small as possible, with the varying
   ! payload sizes moved to separate structures.

   subroutine setvalue(xs, n)
      implicit none
      type(list_t), pointer, intent(inout) :: xs
      type(integer), intent(in) :: n

      if (associated(xs % nested)) then
         print *, "*** error *** assigning value to nesting node"
         stop 1
      end if

      ! allocate storage for the atom if needed. if there is already an atom, overwrite
      ! its value.

      if (.not. associated(xs % atom)) allocate (xs % atom)
      xs % atom % isnull = .false.
      xs % atom % val = n
   end subroutine setvalue

   function getvalue(xs) result(res)
      implicit none
      integer :: res
      type(list_t), pointer, intent(in) :: xs

      res = xs % atom % val
   end function getvalue

   ! list nodes chain togeter to form the actual list
   ! subroutine setnext(xs, ys)
   !    implicit none
   !    type(list_t), pointer, intent(inout) :: xs
   !    type(list_t), pointer, intent(in) :: ys
   !    xs%next => ys
   ! end subroutine setnext

   ! expand by null: creates a new empty list node, links it to xs,
   ! and advances xs.

   subroutine addnewempty(xs)
      implicit none

      type(list_t), pointer, intent(inout) :: xs

      allocate (xs % next)

      xs => xs % next
   end subroutine addnewempty

   ! various list node predicates
   ! i prefer functions to having all the root%member%member%member visible topside

   function isempty(x) result(res)
      implicit none
      logical :: res
      type(list_t), pointer, intent(in) :: x

      res = .not. (associated(x % atom) .or. associated(x % nested))
   end function isempty

   function hasvalue(x) result(res)
      implicit none
      logical :: res
      type(list_t), pointer, intent(in) :: x

      res = associated(x % atom)
   end function hasvalue

   function hasnested(x) result(res)
      implicit none
      logical :: res
      type(list_t), pointer, intent(in) :: x

      res = associated(x % nested)
   end function hasnested

   ! function hasnext(x) result(res)
   !    implicit none
   !    logical :: res
   !    type(list_t), pointer, intent(in) :: x
   !    res = associated(x%next)
   ! end function hasnext

   function isnilorempty(x) result(res)
      implicit none
      logical :: res
      type(list_t), pointer, intent(in) :: x

      res = .not. associated(x)
      if (.not. res) res = isempty(x)
   end function isnilorempty

   function notnilorempty(x) result(res)
      implicit none
      logical :: res
      type(list_t), pointer, intent(in) :: x

      res = .not. isnilorempty(x)
   end function notnilorempty

   ! release the list any linked atom values and sub lists, clearing any storage
   ! before it is released.

   recursive subroutine freelist(xs)
      implicit none
      type(list_t), pointer, intent(inout) :: xs
      type(list_t), pointer :: nx

      assocloop: do while (associated(xs))
         nx => xs % next

         ! free any children
         if (hasnested(xs)) then
            call freelist(xs % nested)
            nullify (xs % nested)
         end if

         ! free any atom
         if (associated(xs % atom)) then
            xs % atom % isnull = .true.
            xs % atom % val = 0
            deallocate (xs % atom)
         end if

         ! release
         nullify (xs % next)
         deallocate (xs)

         ! advance
         xs => nx
      end do assocloop

   end subroutine freelist

   ! create a new list from a value
   function listfromvalue(x) result(res)
      implicit none
      type(list_t), pointer :: res
      type(list_t), pointer, intent(in) :: x

      allocate (res)
      if (hasvalue(x)) call setvalue(res, getvalue(x))
   end function listfromvalue

   ! recursive helper for strlist. should be called with pos pointing at a list
   ! open bracket [ and will return as each list close bracket ] is reached. if
   ! everything is nested properly, it works. there is no attempt at input
   ! validation.

   recursive function strlistr(pos, str) result(res)
      implicit none
      type(list_t), pointer :: res
      character(len=*), intent(in) :: str
      integer, intent(inout) :: pos

      ! xs and ys are modifiable work pointers
      type(list_t), pointer :: xs, ys
      integer :: n

      ! our result is the first item, subsequent items are linked
      ! following. xs is bumped as needed.

      allocate (xs)
      res => xs

      ! guard clauses and special cases

      ! should not happen, just return an empty list
      if (len_trim(str) < 2) then
         return
      end if

      ! an actual empty list is legal but won't happen until i clean
      ! up the loader
      if (str(pos:pos) == "[" .and. str(pos + 1:pos + 1) == "]") then
         pos = pos + 2
         return
      end if

      ! this better be a properly bracketed list
      n = len_trim(str)
      if (str(pos:pos) /= "[" .or. str(n:n) /= "]") then
         print *, "*** error *** misplaced open bracket, skipping ahead one from", intstr(pos)
         pos = pos + 1
         return
      end if

      ! consume the opening [

      pos = pos + 1

      ! and scan through the list

      scanner: do while (pos <= len_trim(str))

         ! commas and spaces are all whitespace

         if (str(pos:pos) == " " .or. str(pos:pos) == ",") then
            pos = pos + 1
            cycle scanner
         end if

         ! end of the current (sub) list? if so, bump and return

         if (str(pos:pos) == "]") then
            pos = pos + 1
            return ! <---------- return from function
         end if

         ! start of a nested (sub) list? if so, call recursively and tack it on to
         ! the list i'm working on.

         if (str(pos:pos) == "[") then
            ys => strlistr(pos, str)
            call setnested(xs, ys)
            call addnewempty(xs)
            cycle scanner
         end if

         ! add atom to list, only ints are supported

         if (str(pos:pos) >= "0" .and. (str(pos:pos) <= "9")) then
            read (str(pos:len_trim(str)), *) n
            call setvalue(xs, n)
            call addnewempty(xs)
            pos = pos + max(1, scan(str(pos:len_trim(str)), " ,[]"))
            cycle scanner
         end if

         ! input format error, report but continue

         print *, "*** error *** at position ", intstr(pos), " in '", trim(str), "'"
         pos = pos + 1

      end do scanner

      print *, "*** error *** list parse failure or unbalanced brackets"
      stop 1
   end function strlistr

   ! from a list of integers and nested lists of integers in string form,
   ! create a navigable set of nodes. the ugly details are in a recursive
   ! helper function.

   function strlist(str) result(res)
      implicit none
      character(len=*), intent(in)  :: str
      type(list_t), pointer         :: res
      character(len=:), allocatable :: sanitized
      integer                       :: pos, i, j, k

      ! read() in strlistr wants a space or comma after integers, not a ] so pad
      ! [ and ] with spaces

      ! how many brackets are there?

      k = 0
      do i = 1, len_trim(str)
         if (str(i:i) == "[") k = k + 1
         if (str(i:i) == "]") k = k + 1
      end do

      ! get enough room for the padding

      allocate (character(len=len_trim(str) + k*3) :: sanitized)
      sanitized = repeat(" ", len_trim(str) + k * 3)

      ! and pad each bracket with a space on either side.

      j = 1
      sanitizer: do i = 1, len_trim(str)
         if (str(i:i) == "[") then
            sanitized(j:j + 2) = " [ "
            j = j + 3
            cycle sanitizer
         end if
         if (str(i:i) == "]") then
            sanitized(j:j + 2) = " ] "
            j = j + 3
            cycle sanitizer
         end if
         sanitized(j:j) = str(i:i)
         j = j + 1
      end do sanitizer

      ! call recursive helper to parse into a list

      pos = 1
      res => strlistr(pos, adjustl(sanitized))

      ! i'm not sure this is needed since we're going out of scope, but
      ! my habit is to release dynamic memory while running.

      deallocate (sanitized)

   end function strlist

   ! given a list of nodes, return a string representation.
   ! overallocating strings just to get this working quickly.

   recursive function liststr(xin) result(res)
      implicit none
      type(list_t), pointer, intent(in) :: xin
      character(len=4096)               :: res
      type(list_t), pointer             :: xs
      integer                           :: i

      ! copy to local so we can step through the list
      xs => xin

      res = "["

      do while (notnilorempty(xs))

         if (hasvalue(xs)) then
            ! this leaves a dangling comma
            res = trim(res)//trim(intstr(getvalue(xs)))//","
            xs => xs % next
            cycle
         end if

         if (hasnested(xs)) then
            res = trim(res)//trim(liststr(getnested(xs)))
            xs => xs % next
            cycle
         end if

         ! i don't think we should get here, but ...

         print *, "*** error *** listtstr empty node, skipping"
         xs => xs % next

      end do

      ! if there's a dangling comma, remove it

      i = len_trim(res)
      if (res(i:i) == ",") res(i:i) = " "

      ! close the bracket and we're done
      res = trim(res)//"]"

   end function liststr

   ! a not so quick exchange sort of the packets into ascending order

   subroutine sort
      implicit none
      integer               :: i
      logical               :: f
      type(list_t), pointer :: xs, ys
      type(list_t)          :: swap

      f = .true.
      outer: do while (f)
         f = .false.
         inner: do i = 1, numpackets - 1
            xs => packet(i); ys => packet(i + 1)
            if (listcompare(xs, ys) > 0) then
               f = .true.
               swap = packet(i)
               packet(i) = packet(i + 1)
               packet(i + 1) = swap
            end if
         end do inner
      end do outer
   end subroutine sort

   ! find a matching list value in the packets and return it index. if no match
   ! is found -1 is returned. while the packets array is sorted for this problem,
   ! this search does not count on the items being in order.

   function indexof(xin) result(res)
      implicit none
      type(list_t), pointer, intent(in) :: xin
      integer                           :: res
      type(list_t), pointer             :: ys
      integer                           :: i

      res = -1
      do i = 1, numpackets
         ys => packet(i)
         if (listcompare(xin, ys) == 0) then
            res = i
            return
         end if
      end do
   end function indexof

end program solution
