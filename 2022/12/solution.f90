! solution.f90 -- 2022 day 12 -- hill climbing algorithm -- troy brumley blametroi@gmail.com

! advent of code 2022 day 12 -- hill climbing algorithm
!
! go from S to E without breaking out your climbing gear. this means you can
! only increase altitude one level per move. you can decrease any number of
! levels though.
!
! the grid is an altitude map or contour map. a is the lowest altitude, z is the
! highest. we assume that S <= a, and E > z from the problem statement.
!
! our sample input:
!
! Sabqponm
! abcryxxl
! accszExk
! acctuvwj
! abdefghi
!
! you can move only in the four cardinal directions.

! priority queue, and so on. there will be pointers. they are different in
! fortran from what i am used to.

program solution

   use iso_fortran_env, only: int64
   use aocinput, only: open_aoc_input, read_aoc_input, close_aoc_input, max_aoc_reclen, rewind_aoc_input

   implicit none

   integer, parameter            :: AOCIN = 10
   integer(kind=int64)           :: part_one, part_two  ! results
   character(len=max_aoc_reclen) :: rec

   integer, parameter :: MAXCOLS = 96   ! by observation plenty big enough
   integer, parameter :: MAXROWS = 96

   ! iirc i didn't use move or glyph much
   type cell_t
      integer          :: r, c        ! who am i, where am i?
      integer          :: altitude    ! a variable permeability wall in the maze
      integer          :: distance    !
      logical          :: n, e, s, w  ! is a path available in that direction?
      character(len=1) :: glyph       ! displayable representation
      character(len=1) :: move        ! move logged for final pass
   end type cell_t

   ! a queue of grid cells to visit, or that have been visted
   type cellref_t
      type(cell_t), pointer :: cell
      type(cellref_t), pointer :: next
   end type cellref_t

   ! global state, as is my usual, the array bounds are padded on either end for coding simplicity
   type(cell_t), target     :: grid(0:MAXROWS + 1, 0:MAXCOLS + 1)
   integer                  :: begr, begc        ! our path starts here
   integer                  :: endr, endc        ! and ends here
   integer                  :: minalt, maxalt    ! altitude ranges
   type(cellref_t), pointer :: workq, visited
   integer                  :: srow, scol

   ! input file is argument 1
   rec = ""
   call open_aoc_input(AOCIN)

   ! initialize
   part_one = 0; part_two = 0
   nullify (workq); nullify (visited)

   call init
   call load

   part_one = search(begr, begc);
   part_two = MAXROWS*MAXCOLS + 1
   do srow = 1, MAXROWS
      do scol = 1, MAXCOLS
         if (grid(srow, scol)%glyph == "a") part_two = min(search(srow, scol), part_two)
      end do
   end do

   print *
   print *, part_one, "part one"
   print *, part_two, "part two"
   print *

   call close_aoc_input(AOCIN)

contains

   subroutine init
      implicit none
      integer :: r, c
      do r = 0, MAXROWS + 1
         do c = 0, MAXCOLS + 1
            grid(r, c)%r = r; grid(r, c)%c = c
            grid(r, c)%n = .false.
            grid(r, c)%e = .false.
            grid(r, c)%s = .false.
            grid(r, c)%w = .false.
            grid(r, c)%altitude = 100 ! higher than anything else
            grid(r, c)%glyph = "~" ! outside the range of our input, high
            grid(r, c)%move = " "
            grid(r, c)%distance = MAXROWS*MAXCOLS ! a high value
         end do
      end do
      begr = -1; begc = -1
      endr = -1; endc = -1
      minalt = 10000
      maxalt = -minalt
   end subroutine init

   subroutine load
      implicit none
      integer :: i, j, k
      call rewind_aoc_input(AOCIN)

      ! load it up, minimal parsing needed
      rec = ""
      i = 0
      do while (read_aoc_input(AOCIN, rec))
         if (len_trim(rec) == 0) cycle
         i = i + 1
         do j = 1, len_trim(rec)
            grid(i, j)%r = i; grid(i, j)%c = j
            grid(i, j)%glyph = rec(j:j)
            if (rec(j:j) == "S") then
               begr = i; begc = j
               grid(i, j)%altitude = 0
               cycle
            end if
            if (rec(j:j) == "E") then
               endr = i; endc = j
               grid(i, j)%altitude = ichar("z") - ichar("a")
               cycle
            end if
            grid(i, j)%altitude = ichar(rec(j:j)) - ichar("a")
            minalt = min(minalt, grid(i, j)%altitude);
            maxalt = max(maxalt, grid(i, j)%altitude);
         end do
      end do

      ! each grid cell has an altitude, we can only move to a lower altitude,
      ! the same altitude, or at most one level higher. the altitude becomes the
      ! wall of this maze.
      !
      ! by having boundary cells configured properly, there is no special edge
      ! logic needed when checking for a move.
      do i = 1, MAXROWS
         do j = 1, MAXCOLS
            k = grid(i, j)%altitude
            grid(i, j)%n = (grid(i - 1, j)%altitude - k) < 2
            grid(i, j)%e = (grid(i, j + 1)%altitude - k) < 2
            grid(i, j)%s = (grid(i + 1, j)%altitude - k) < 2
            grid(i, j)%w = (grid(i, j - 1)%altitude - k) < 2
         end do
      end do

      if ((begr == -1) .or. (endr == -1)) then
         print *, "*** error *** begin or end grid was not found"
         stop 1
      end if
      nullify (workq)
      nullify (visited)
   end subroutine load

   ! release all references on the passed cell list. in the pascal version, the
   ! list anchor is not cleared by this operation, but its storage is released.
   ! if i'm reading this right, deallocate implies nullify, so i've marked the
   ! intent as inout instead of just in.
   subroutine freerefs(p)
      implicit none
      type(cellref_t), pointer, intent(inout) :: p
      type(cellref_t), pointer :: n

      do while (associated(p))
         n => p%next
         deallocate (p)
         p => n
      end do
   end subroutine freerefs

   ! create a new reference to a cell in the grid and set it's distance at this
   ! time.
   function makeref(r, c, d) result(res)
      implicit none
      type(cellref_t), pointer :: res
      integer, intent(in) :: r, c, d
      allocate (res)
      res%cell => grid(r, c)
      nullify (res%next)
      grid(r, c)%distance = d
   end function makeref

   ! is a cell reference with these coordinates already on the passed list?
   function alreadyon(p, nr, nc) result(res)
      implicit none
      logical :: res
      type(cellref_t), pointer, intent(in) :: p
      type(cellref_t), pointer :: mp
      integer, intent(in) :: nr, nc
      ! am i allowed to modify my local copy of an in argument?
      ! i am not. that's fine.
      mp => p
      res = .false.
      do while (associated(mp) .and. .not. res)
         res = ((mp%cell%r == nr) .and. (mp%cell%c == nc))
         mp => mp%next
      end do
   end function alreadyon

   ! add this reference to the end of the work queue.
   subroutine addref(r)
      implicit none
      type(cellref_t), pointer, intent(in) :: r
      type(cellref_t), pointer :: p
      if (.not. associated(workq)) then
         workq => r
         return
      end if
      p => workq
      do while (associated(p%next))
         p => p%next
      end do
      p%next => r
   end subroutine addref

   ! search to find the path to the end of the maze. a bit of djikstra goes
   ! just far enough.
   recursive function search(row, col) result(res)
      implicit none
      integer :: res
      integer, intent(in) :: row, col
      integer :: nr, nc
      type(cellref_t), pointer :: curr, ref

      ! default ... not found
      res = MAXROWS*MAXCOLS + 1

      ! free prior work q and visited lists
      call freerefs(workq)
      nullify (workq)
      call freerefs(visited)
      nullify (visited)

      ! put the first cell reference on the work q
      curr => makeref(row, col, 0)
      workq => curr

      ! and search until work q is exhausted
      do while (associated(workq))

         ! pop
         curr => workq
         workq => workq%next

         !push
         curr%next => visited
         visited => curr

         ! checking each direction. my backing data structure trapped me into this
         ! bit of repetition, but it's not so bad.

         ! north
         if (curr%cell%n) then
            nr = curr%cell%r - 1; nc = curr%cell%c
            if (grid(nr, nc)%glyph == "E") then
               res = curr%cell%distance + 1
               return
            end if
            if (.not. (alreadyon(workq, nr, nc) .or. alreadyon(visited, nr, nc))) then
               ref => makeref(nr, nc, curr%cell%distance + 1)
               call addref(ref)
            end if
         end if

         ! east
         if (curr%cell%e) then
            nr = curr%cell%r; nc = curr%cell%c + 1
            if (grid(nr, nc)%glyph == "E") then
               res = curr%cell%distance + 1
               return
            end if
            if (.not. (alreadyon(workq, nr, nc) .or. alreadyon(visited, nr, nc))) then
               ref => makeref(nr, nc, curr%cell%distance + 1)
               call addref(ref)
            end if
         end if

         ! south
         if (curr%cell%s) then
            nr = curr%cell%r + 1; nc = curr%cell%c
            if (grid(nr, nc)%glyph == "E") then
               res = curr%cell%distance + 1
               return
            end if
            if (.not. (alreadyon(workq, nr, nc) .or. alreadyon(visited, nr, nc))) then
               ref => makeref(nr, nc, curr%cell%distance + 1)
               call addref(ref)
            end if
         end if

         ! west
         if (curr%cell%w) then
            nr = curr%cell%r; nc = curr%cell%c - 1
            if (grid(nr, nc)%glyph == "E") then
               res = curr%cell%distance + 1
               return
            end if
            if (.not. (alreadyon(workq, nr, nc) .or. alreadyon(visited, nr, nc))) then
               ref => makeref(nr, nc, curr%cell%distance + 1)
               call addref(ref)
            end if
         end if

      end do
      ! and keep looping until work queue is empty
      ! if we get here, there is no path from the requested location
   end function search
end program solution
