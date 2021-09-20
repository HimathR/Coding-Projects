// HEADER FILES //
#include <iostream>
#include <algorithm> // for sort
#include <fstream>   // for file io
#include <vector>    // for a constructor
using namespace std;

// ** CLASS SPECIFICATIONS ** //
class Alist
{
private:
  int capacity = 8, initial_size = 8; // default capacity (current size of array)
  int *nums;                          // the actual array for each object
  bool sorted = false;                // whether the object is sorted or not
  int count = 0;                      // number of ints in the array

  void change_size();                // function for the process of changing size in extend, shrink, resize functions
  void capacitycheck(Alist &newobj); // checks a locally defined object in operators to ensure it hasn't exceeded capacity
  bool equals(Alist &rhs);           // for the != and == operator to reduce duplicate code
public:
  //  ** CONSTRUCTORS **  //
  Alist(const Alist &obj); // Copy Constructor
  Alist(Alist &&obj);      // Move Constructor
  Alist()                  // Default Constructor - no parameters
  {
    capacity = 8, initial_size = 8; // set default capacity/size
    nums = new int[capacity];       // allocate memory
  }
  Alist(bool s) : sorted(s) // constructor with data types for a2
  {
    capacity = 8, initial_size = 8;
    nums = new int[capacity];
  }
  Alist(int n, bool s) : capacity(n), sorted(s) // constructor with data types for a3
  {
    initial_size = n;
    nums = new int[capacity];
  }
  Alist(const char *filename) // constructor with data types for a5
  {
    reload(filename);
  }
  Alist(vector<int> vn);

  // **     OVERLOADS     ** //
  Alist &operator=(const Alist &obj); // copy assignment operator
  Alist operator=(Alist &&rhs);       // move assignment operator
  int operator[](int i);
  Alist &operator+=(int n);
  Alist &operator-=(int n);
  bool operator!=(Alist &rhs);
  bool operator==(Alist &rhs);
  Alist operator+(const Alist rhs);
  Alist operator&&(Alist &rhs);
  Alist operator^(Alist &rhs);
  Alist operator-(const Alist &rhs);
  friend ostream &operator<<(ostream &os, Alist &obj);

  // **     FUNCTIONS     ** //
  void extend();
  void shrink();
  void resize(int n);
  int get_size() const { return capacity; }
  int get_count() const { return count; }
  int compare();
  void sort();
  void unsort();
  int remove(int n);
  int search(int n);
  void save(const char *filename);
  void reload(const char *filename);

  // DESTRUCTOR //
  ~Alist() { delete[] nums; }
};

// ** CONSTRUCTOR DEFINITIONS ** //
Alist::Alist(const Alist &obj) // Copy Constructor
{
  capacity = obj.capacity;        // copy across default capacity
  count = obj.count;              // copy across number of ints in array
  sorted = obj.sorted;            // copy across whether object is sorted or not
  nums = new int[capacity];       // make new numbers array
  for (int i = 0; i < count; i++) // copy array data across
    nums[i] = obj.nums[i];
}
Alist::Alist(Alist &&obj) // Move Constructor
{
  nums = obj.nums;    // copy data back
  obj.nums = nullptr; // reset other object's nums pointer
  capacity = obj.capacity;
  obj.capacity = 0;
  count = obj.count;
  obj.count = 0;
}
Alist::Alist(vector<int> vn)
{
  int n = vn.size();
  nums = new int[n];
  for (int i = 0; i < n; i++)
    nums[i] = vn[i];
}

// ** CLASS DEFINITIONS ** //
// **     OVERLOADS     ** //
Alist &Alist::operator=(const Alist &obj) // copy assignment operator for previously initialized object
{
  if (this != &obj)
  {
    capacity = obj.capacity;
    count = obj.count;
    sorted = obj.sorted;
    nums = new int[capacity];
    for (int i = 0; i < count; i++)
      nums[i] = obj.nums[i];
  }
  return *this;
}

Alist Alist::operator=(Alist &&rhs) // move assignment operator
{
  if (this != &rhs)
  {
    delete[] nums;      // delete original data for object
    nums = rhs.nums;    // copy data back
    rhs.nums = nullptr; // reset other object's nums pointer
    capacity = rhs.capacity;
    rhs.capacity = 0;
    count = rhs.count;
    rhs.count = 0;
  }
  return *this;
}

Alist &Alist::operator+=(int n)
{
  if (count == capacity) // check to make sure capacity has not been hit
    this->extend();      // if capacity has been hit, extend the object's size
  if (sorted)
  {
    int i;
    for (i = count - 1; (i >= 0 && nums[i] > n); i--)
      nums[i + 1] = nums[i];
    nums[i + 1] = n;
  }
  else
    nums[count] = n; // add n into the array at the current index (equivalent to current count)
  count++;           // increment count as a new int n is to be added
  return *this;      // return the array
}

int Alist::operator[](int i)
{
  if (i >= capacity || i < 0) // check whether value is within bounds
  {
    cout << "Invalid Index: " << i << endl;
    return 0;
  }
  return nums[i];
}

Alist &Alist::operator-=(int n)
{
  remove(n); 
  return *this;
}

bool Alist::operator!=(Alist &rhs) // != and == are related
{
  return !equals(rhs);
}

bool Alist::operator==(Alist &rhs)
{
  return equals(rhs);
}

Alist Alist::operator+(const Alist rhs)
{
  Alist newobj(*this);            // copy values for obj on left hand side
  for (int i = 0; i < count; i++) // add in values for right hand side object
    newobj += rhs.nums[i];
  return newobj; // returning by value, not reference
}

Alist Alist::operator&&(Alist &rhs)
{
  Alist newobj;
  for (int i = 0; i < count; i++)
  {
    int n = nums[i];         // for each value in list a
    rhs.search(n);           // search it in list b
    if (rhs.search(n) != -1) // if value is found
      newobj += n;           // put it into newobj array
  }
  return newobj;
}

Alist Alist::operator^(Alist &rhs)
{
  Alist newobj;
  for (int i = 0; i < count; i++)
  {
    int n = nums[i];         // for each value in list a
    rhs.search(n);           // search it in list b
    if (rhs.search(n) == -1) // if value is NOT found
      newobj += n;           // put it into newobj array
    int m = rhs.nums[i];     // for each value in list b
    search(m);               // search it in list a
    if (search(m) == -1)     // if value is NOT found
      newobj += m;           // put it into newobj array
  }
  return newobj;
}

Alist Alist::operator-(const Alist &rhs)
{
  Alist newobj;
  int i = 0, j = 0, k = 0;
  while (i < count && j < rhs.count)
  {
    if (nums[i] < rhs.nums[j]) // if not in both, add the smaller number to newobj array
    {
      newobj.nums[k] = nums[i];
      i++, k++, newobj.count++;
      capacitycheck(newobj);
    }
    else if (rhs.nums[j] < nums[i])
    {
      newobj.nums[k] = rhs.nums[j];
      j++, k++, newobj.count++;
      capacitycheck(newobj);
    }
    else // if in both, just iterate again
      i++, j++;
  }
  return newobj;
}

ostream &operator<<(ostream &os, Alist &obj)
{
  for (int i = 0; i < obj.get_count(); i++)
  {
    os << obj.nums[i] << " ";
  }
  return os;
}

// **     FUNCTIONS     ** //
void Alist::change_size()
{
  int *temp_array = new int[capacity]; // allocate memory for nums, with the newly extended capacity
  for (int i = 0; i < count; i++)      // copy data from temp array to numbers by indexing through
  {
    temp_array[i] = nums[i];
  }
  delete[] nums; // delete memory allocated for temp array
  nums = temp_array;
}

void Alist::resize(int n)
{
  capacity = n;         // reallocate for array size
  if (count > capacity) // when count is greater than capacity, set the count to capacity
    count = capacity;
  change_size();
}

void Alist::extend()
{
  capacity += initial_size; // increase capacity variable by the initial_size previously found
  change_size();
}

void Alist::shrink() // apply same logic as extend function, but this time subtract initial size instead
{
  capacity -= initial_size; // increase capacity variable by the initial_size previously found
  change_size();
}

void Alist::sort()
{
  sorted = true; // sorted is now true for that object
  std::sort(nums, nums + count);
}

void Alist::unsort()
{
  sorted = false;
  for (int i = count - 1; i > 0; i--)
    swap(nums[i], nums[rand() % count]);
}

int Alist::remove(int n)
{
  bool found = false;
  int x = search(n); // search for the value in question
  if (sorted)
  {
    if (x != -1) // if a value is found
    {
      found = true;
      for (int i = x; i < count - 1; i++) // remove found value
        nums[i] = nums[i + 1];
      count--;
    }
    else
      found = false;
  }
  else // if unsorted
  {
    if (x != -1)
    {
      found = true;
      nums[x] = nums[count];
      count--;
    }
    else
      found = false;
  }
  if (count <= capacity - initial_size) // if # elements is less than or equal to (capacity - default capacity), shrink
    shrink();
  return found;
}

int Alist::search(int n)
{
  if (sorted) // if array is sorted, a binary search will run
  {
    int l = 0;        // L is the beginning index to middle
    int r = capacity; // R is the middle to the end index (capacity of object)
    while (l <= r)
    {
      int middle = l + (r - l) / 2;
      if (nums[middle] == n) // check to see if x is in the middle
        return middle;
      if (nums[middle] < n) // check to see if given number greater than than the middle
        l = middle + 1;     // if so, move to only search in the left half
      else                  // if given number is less than middle,
        r = middle - 1;     // move to search in right half
    }
  }
  else // otherwise, check each integer in array if unsorted
  {
    for (int i = 0; i < count; i++)
    {
      if (nums[i] == n)
        return i;
    }
  }
  // if by this point no integer was found, return -1 as specified in task sheet
  return -1;
}

void Alist::capacitycheck(Alist &newobj) // function used to reduce duplicate code
{
  if (newobj.count == newobj.capacity)
    newobj.extend();
}

bool Alist::equals(Alist &rhs) // function for the == and != operator
{
  bool identical = true;
  for (int i = 0; i < count; i++)
  {
    if (nums[i] != rhs.nums[i])
      identical = false;
  }
  if (identical && capacity == rhs.capacity && count == rhs.count && sorted == rhs.sorted)
    return true; // if all array variables are copied correctly, the == relationship is true
  else
    return false; // otherwise return false, (the != relationship is true)
}

void Alist::save(const char *filename)
{
  ofstream file;
  file.open(filename); // open a file for writing to
  if (file.is_open())
  {
    // write important variables to file
    file << capacity << " " << initial_size << " " << count << " " << sorted << " ";
    for (int i = 0; i < count; i++)
      file << nums[i] << " ";
    file.close();
  }
  else
    cout << "Unable to open file";
}

void Alist::reload(const char *filename)
{
  ifstream file(filename);
  if (file.is_open())
  {
    file >> capacity >> initial_size >> count >> sorted; // read in variables in saved order, as well as list of nums
    nums = new int[capacity];
    for (int i = 0; i < count; i++)
      file >> nums[i];
  }
  else
    cout << "Unable to open file";
}

// MAIN FUNCTION //
int main()
{ // Test basic constructors and += overloading
  Alist a1, a2(true), a3(72, false), a4, a6, a7, a8;
  const int nums_count = 100;
  int n;
  for (int i = 0; i < nums_count; i++)
  {
    n = rand() % 1000;
    a1 += n;
    a2 += n;
    a3 += n;
  }
  cout << "Sizes: " << a1.get_size() << " " << a2.get_size() << " " << a3.get_size() << endl;
  cout << "Counts: " << a1.get_count() << " " << a2.get_count() << " " << a3.get_count() << endl;

  // Test sorting
  bool sorted = true;
  for (int i = 1; i < a2.get_count() && sorted; i++)
    if (a2[i - 1] > a2[i])
      sorted = false;
  if (!sorted)
    cout << "a2 not initialised to sorted" << endl;
  a1.sort();
  for (int i = 1; i < nums_count; i++)
    if (a1[i - 1] > a1[i])
    {
      cout << "a1 not sorted" << endl;
      break;
    }
  a2.unsort();
  sorted = true;
  for (int i = 1; i < a2.get_count() && sorted; i++)
    if (a2[i - 1] > a2[i])
      sorted = false;
  if (sorted)
    cout << "Unsorted failed" << endl;

  // Test explicit methods and -= overloading
  n = a1[20];
  cout << "Double remove: " << a1.remove(n) << " " << a1.remove(n) << endl;
  cout << "Search present, not present: " << a1.search(a1[50]) << " " << a1.search(n) << endl;
  a1 += n;
  cout << "Search (should be 20 57): " << a1.search(n) << " " << a2.search(n) << endl;
  a1.extend();
  cout << "Extend (should be 112): " << a1.get_size() << endl;
  a1.shrink();
  cout << "Shrink (should be 104): " << a1.get_size() << endl;
  n = a1.get_size();
  for (int i = 0; i < n - 6; i++)
    a1 -= a1[0];
  cout << "Remove (should be 8): " << a1.get_size() << endl;
  a3.save("nums.txt");
  Alist a5("nums.txt");
  if (a3 != a5)
    cout << "save / reload failed" << endl;

  // Test copy constructors and resize
  a4 = a3;
  if (a4 != a3)
    cout << "Copy Constructor 1 Fail" << endl;
  a4.remove(a4[50]);
  if (a4 == a3)
    cout << "Copy Constructor 2 Fail" << endl;

  a4.resize(50);
  cout << "Resize (should be 50 50): " << a4.get_size() << " " << a4.get_count() << endl;

  // Test other operator overloading
  n = a1[-20];
  n = a1[2 * nums_count];

  for (int i = 0; i < 20; i += 2)
    a6 += i;
  for (int i = 1; i < 20; i += 2)
    a7 += i;
  a5 = a6 + a7;
  a5.sort();
  for (int i = 0; i < 20; i++)
    if (a5[i] != i)
      cout << "+ operator failed" << endl;
  a8 = a6 && a7;
  if (a8.get_count() != 0)
    cout << "&& operator failed 1" << endl;
  a8 = a6 && a6;
  if (a8.get_count() != a6.get_count())
    cout << "&& operator failed 2" << endl;
  a8 = a6 ^ a7;
  if (a8.get_count() != 20)
    cout << "^ operator failed 1" << endl;
  a8 = a6 ^ a6;
  if (a8.get_count() != 0)
    cout << "^ operator failed 2" << endl;
  a8 = a5 - a7;
  a8.sort();
  for (int i = 0; i < 20; i += 2)
    if (a8[i / 2] != i)
      cout << "- operator failed" << endl;
  a7 += 28;
  if (a7.search(28) == -1)
    cout << "+= operator failed" << endl;
  a7 -= 19;
  if (a7.search(19) != -1)
    cout << "-= operator failed" << endl;

  // Test other constructors
  Alist a9(a8);
  a9.remove(8);
  if (a9.get_count() != a8.get_count() - 1)
    cout << "Constructor 1 Fail" << endl;
  cout << a8 << endl;
  cout << a9 << endl;
  vector<int> vn{0, 1, 2, 3, 4};
  Alist a10(vn);
  for (int i = 0; i < vn.size(); i++)
    if (a10[i] != vn[i])
      cout << "Constructor 2 Fail" << endl;
  return 0;
}