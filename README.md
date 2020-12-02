# Final Project

**Group name:** ThreadMastas  

**Group Members:**
 - Andey Tuttle  
 - Stefan Emmons  
 - Will Malone  

**What is working and not working?**
 - Drivers 1-6 are working.
 - Compile with the makefile in the `/project` directory.

**What are your attributes?**
 - Last Accessed Date  
 - Emoticons
 
 ---

**grading is out of 100 points**
* create file (imples r/w in dm, pm r/w, getfree, and returnblock) (10 points):  yes
* openfile (and close 3points of) opentable, etc ignoring unlock/lock (10 points):  yes
* readfile (10 points):    yes
* writefile (10 points):  yes
* seekfile (5 points):   yes
* appendfile ( 5 points, since just call see to end and write):  yes
  * worked harder then necessary, should have just called seekfile with the size, then write.
* createdir (10 points) => implies create,open work as well:     yes
* lock and unlock (5 each)  looped into open/close as well:   yes
* rename (5 points):   yes
* deletefile (5 points), remember lock again:   yes
* deletedir (5 points), remember empty:  yes
* attributes read/set (5 points each):  yes

other notes:
  * disk is 
    * driver 1:  over all looks good.
      * default attributes?
      * bitvector is read into an not intialized buffer, so lot's of junk in it.
      * odd decision in superblock not to use 4byte numbers, when you use it everywhere else.
    * driver 2:  looks good.
      * all paritions have data, bvs has junk data.
    * driver 3: looks good.
    * driver 4: I have a concern about when the directory fills up in partition 3.
    * driver 5: everything looks good here, so d4 works.
    * driver 6: get and set look good.  
  * other
    * code looks good.  good helper functions.  
    * why are you Deriving class, but adding nothing?  instead just use the base class, example Lockedfiles?
  * base grade: 100
  * group eval:  individual grades will be emailed.
