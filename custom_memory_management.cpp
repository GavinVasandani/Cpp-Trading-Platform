//Program where I created a custom memory pool (Arena) and a memory allocation strategy for memory of the pool.
//Program also has a custom allocator that is optimized for small sized containers, wherein its more quicker to store values in stack with pre-determined stack frame allocated for container even though container size is dynamic,
//than allocating on heap.
//This custom allocator is used when allocating memory for a small sized vector object as an example.
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>

using namespace std;

//Modifying find_if algo:
template <class InputIt, class T>
constexpr bool find_if(InputIt first, InputIt last, T* p) { //p is pntr pointing to certain type
//iterators just increment 
    for(; first!=last; first++) {
        if(p==&(*first)) {return true;}
    }
    return false;
}

template <size_t N>
class Arena { //user defined type, object of this type is an instance of a memory pool 
    static constexpr size_t alignment = alignof(max_align_t); //alignof(max_align_t) is alignment value of the max variable/object (not sure of what)
    
    public:
        Arena() noexcept : ptr_(buffer_) {} //constructor 1 (no input param) assigning buffer to ptr
        Arena(const Arena&) = delete; //constructor 2 (arena object input tpye), calls deconstructor so: Arena(Arena_Object_Name); this destroys Arena_Object_Name instance

        Arena& operator=(const Arena&) = delete; //operator overload for = Arena_Object_Name, which calls deconstructor to destroy Arena_Object_Name instance

        auto reset() noexcept {ptr_ = buffer_;}
        static constexpr auto size() noexcept {return N;}
        auto used() const noexcept {
            return static_cast<size_t>(ptr_ - buffer_); //ptr_, buffer_ are byte* ptr types so (-) applies pointer arithmetic and output is type casted to size_t type.
        }
        
        auto allocate(size_t n) -> byte*;
        //try:
        //byte* allocate (size_t n);
        //void deallocate(byte* p, size_t n) noexcept
        auto deallocate(byte* p, size_t n) -> void;
        
        private:
            static auto align_up(size_t n) -> size_t {
                return (n+(alignment-1)) & ~(alignment-1);
            }
            auto pointer_in_buffer(const byte* p) const noexcept -> bool {
                //return find_if(buffer_.begin(), buffer_.end(), p); 
                //should use user-defined find_if as we used different parameter types then algorithm::find_if();
                //This assumes buffer is container and not pointer type
                //or
                return p >= buffer_ && p < buffer_ + N; //buffer is address to first mem location in arena, so range is buffer_ < p < buffer_ + N, where N is size of arena
            }
            alignas (alignment) byte buffer_[N];
            byte* ptr_ {};
};

template <size_t N>
auto Arena<N>::allocate(size_t n) -> byte* {
    const auto aligned_n = align_up(n);
    const auto available_bytes = static_cast <decltype(aligned_n)> (buffer_ + N - ptr_);

    if(available_bytes >= aligned_n) {
        auto* r = ptr_;
        ptr_ = ptr_ + aligned_n;
        return r;
    }
    return static_cast<byte*>(::operator new(n));
}

template <size_t N> //deallocate func is still member func in a class template, not an initialized class
//only when we do: Arena<5> objectName; the class template is initialized
auto Arena<N>::deallocate(byte* p, size_t n) -> void { //pntr p refers to mem location to delete from, size_t n is bytes to delete (inferred from data type or specificed if want to deallocate contig memory
    //Checking if pntr p is assigned mem address that is in arena:
    //pointer_in_buffer is private func, so it can be used by public/private member func of arena class but not by objects, so objectName.pointer_in_buffer isn't possible
    if (pointer_in_buffer(p)) {
        n = align_up(n); //align_up(n) gives mem locations needed to store dataType of bytes n and for padding so that dataType is aligned
        if (p + n == ptr_) { //checking if object pointed to by p is most recently allocated object.
            ptr_ = p; //so reset ptr_ which is pntr to LS address that's unallocated to p, so unallocates/allows rewriting of memory allocated for object pointed to by p
            return;
        }
        //if not most recent but in buffer, then this isn't possible case as memory strategy for allocation in arena is same as allocation in stack, so LIFO allocation and deallocation
        cout<<"[WARNING] Must deallocate most recently allocated memory.";
    }
    else {
        ::operator delete(p); //if not in buffer then call global operator delete(p)
    }
}

//Can we allocate memory in arena to an object on program startup even if arena doesn't start at 0 address? Presumably we fill memory in stack up to arena start and then start filling arena.
//Or maybe ptr_ = 0 address in stack, so immediately the object can be allocated on the stack.

//Creating class that overloads new, delete operator to use memory from Arena object:
Arena<1024> testArena;
class User {
    private:
        //Arena<1024> testArena(); //WRONG, every initialization of object of type User will initialize a new Arena object, so must put outside.
        //auto testArena = Arena<1024> {};
        //This creates object, calls no input param constructor;
        int id;
    
    public:
        auto operator new (size_t size) -> void* {
            return testArena.allocate(size); //outputs address to mem location that allocated memory in Arena
            //testArena is global object so can be referenced in any member func of any class
        }
        auto operator delete (void* p) -> void { //we're allocating objects of type User and destroying object of type User, so sizeof is sizeof(User{}); which is size of object of type User
        //cannot do (*p) when p is type void*, maybe as unknown what pntr p is pointing too so how much memory to allocate (not sure).
            return testArena.deallocate(static_cast<byte*>(p), sizeof(User{}));
        }
        auto operator new[] (size_t size) -> void* { //go over new[]
            return testArena.allocate(size);
        }
        auto operator delete[] (void* p, size_t size) -> void {
            return testArena.deallocate(static_cast<byte*>(p), size);
        }
        auto ArenaSize () -> void {
            cout<<"Size of Arena is: "<<testArena.size()<<endl;
        }
        void usedSize () {
            cout<<"Amount of byte-addressed memory used in arena is: "<<testArena.used()<<endl;
        }
        void resetArena () {
            cout<<"Deallocating all memory locations in arena..."<<endl;
            testArena.reset();
        }
};

//Adding custom memory allocator so that objects of containers are initialized, memory is allocated via custom allocator.
//This custom allocator is optimized for small sized containers, wherein its more quicker to store values in stack with pre-determined stack frame allocated for container even though container size is dynamic,
//than allocating on heap.

template <class T, size_t N>
struct ShortAlloc {

    using value_type = T;
    using arena_type = Arena<N>; //not same as Arena<N> {}; which initializes object of Arena<N> type, this just assigns arena_type which is a type to Arena<N> type.

    private:
        arena_type* arena_; //pointr to mem location which is LS byte address of an arena object

    public:
        ShortAlloc(const ShortAlloc&) = default;
        ShortAlloc& operator=(const ShortAlloc&) = default;

        ShortAlloc(arena_type& arena) : arena_{&arena}
        {}

        template <class U>
        ShortAlloc(const ShortAlloc<U, N>& other) : arena_{other.arena_} //so this reuses ShortAlloc object when allocating next element in container.
        {}

        template <class U> struct rebind {
            using other = ShortAlloc<U, N>;
        };

        //Allocating to custom memory pool (Arena): 
        auto allocate(size_t n) -> T* {
            auto bytePntr = arena_->allocate(n*sizeof(T)); //so allocate infers bytes of element Type and number of elements to be constructed.
            //in this case allocate infers number of elements to be constructed (specified when initializing container object) and T is type of element so 
            //n*sizeof(T) is total bytes to store all elements of container/aka number of contig byte addresses to allocate for container of element type T and container size n;
            //returns pntr to object of type T so must type cast/convert from bytePntr (byte*)

            //return static_cast<T*>(bytePntr); 
            //not converting from void* to a specific pntr type which is safe so static_cast can be used, doing byte* to T*, not necessarily it's safe and pntr will translate well so must use reinterpret_cast for unsafe conversion which is legal:
            return reinterpret_cast<T*>(bytePntr);
       
        }

        auto deallocate(void* p, size_t n) -> void { //p is pntr to mem location in arena to deallocate, n is amount of elements to deallocate
            return arena_->deallocate(static_cast<byte*>(p), n*sizeof(T)); //so deallocating entire container (n*sizeof(T) bytes) and deallocating from mem location given by address p
        }

        template <class U, size_t M> //shortAlloc has template T, N so variables U, M are just different variables to define element type and arena size of other ShortAlloc instance (called other)
        auto operator==(const ShortAlloc<U, M>& other) { //checks if the 2 instances are equal, aka checks if the arena both these allocators are using/pointing to is identical
            return N == M && arena_ == other.arena_; //so checks if sizes of arenas are same and whether arena pointing to is the same, so identical allocators but not necessarily the element constructing U is the same
        }

        template <class U, size_t M>
        auto operator!=(const ShortAlloc<U, M>& other) {
            return !(N == M && arena_ == other.arena_);
        }
        
        template <class U, size_t M>
        friend struct shortAlloc;

};

int main() {

    /*
    new User user1; //so calls default constructor, doesn't matter as value assigned to member variable id isn't used for anything
    delete user1; //can a object named after new operator be used to refer to that object? normally not, but in this case new allocates to stack not heap, so:
    //Actually not possible, output of new is pntr, nothing is assigned to pntr to pntr cannot be referenced again when using in delete operator.
    //user1 is type User not pntr, should be pntr to use as input param for delete operator
    */

    auto user1 = new User{}; //user1 is User* type
    //user1 = User{}; //user1 is User type and user1 refers to object of type User
    user1->ArenaSize();

    cout<<"Memory allocated to Arena"<<endl;

    user1->usedSize();
    delete user1;

    cout<<"Memory after deletion: "<<endl;
    cout<<"Memory used of Arena is: "<<testArena.used()<<endl;

    auto multi_users = new User[10];
    cout<<"Multiple objects allocated to Arena"<<endl;

    multi_users->usedSize();
    multi_users->resetArena();

    cout<<"Memory of arena used after reset: "<<endl;
    multi_users->usedSize(); //might not work as memory for object is deallocated, technically it's just going to be reassigned but isn't actually deallocated.

    delete[] multi_users; //resetArena already deallocates, so pntr is technically still existing but it's not within/less than ptr_ as that has been reset so warning.

    cout<<"End!"<<endl;
    cout<<endl;

    cout<<"Allocating vector, testing if memory allocated on Arena: "<<endl;
    auto userVec = vector<User>{};
    cout<<"Memory used of Arena is: "<<testArena.used()<<endl;
    User* userdArray = new User[10];
    userdArray->usedSize();
    cout<<endl;

    //vector<...> class allocates memory for an underlying dArray by doing:
    //malloc(size_t ...);
    //placement new(...);
    //so allocates memory then constructs object in memory locations, so uses placement new not operator new which allocates and constructs

    //Creating vector object that uses custom allocator:
    //Use this object when constructing ShortAlloc object
    //using SmallVec = vector<int, ShortAlloc<int, 512> (ShortAllocArena)>; //so smallVec is type that is vector class type with element type int and a custom allocator ShortAlloc<elementType = int, ArenaSize = 512 bytes>
    //ShortAlloc has private member variable that points to Arena object but never initializes an arena object so:
    //How does vector use an allocator?

    cout<<"Creating vector object using custom memory allocator that allocates to custom memory pool: "<<endl;
    cout<<endl;
    using SmallVec = vector<int, ShortAlloc<int, 512>>;

    auto stack_arena = SmallVec::allocator_type::arena_type{}; //this enters smallVec namespace, enters allocator_type namespace which is a member variable in vector class. allocator_type is ShortAlloc so enter its namespace and create instance of
    //arena that is Arena<512> this is just same type as arena_type so create arena_type obj: arena_type {}; This obj is assigned to stack_arena so stack_arena is type Arena<512>

    cout<<"Before allocating vector object - memory of ShortAllocArena used: "<<stack_arena.used()<<endl;
    //vector<int, ShortAlloc<int, 512> (ShortAllocArena)> customAllocVec (10); //10 objects of type int allocated on arena, same ShortAlloc object is used everytime we allocate memory for a new element of type Int, so same Arena is being used.
    auto SmallVecObj = SmallVec{stack_arena}; //not sure why in curly brackets;
    //This creates SmallVec object which is object/container of type vector with elements of type int and custom allocator is used, memory pool/arena is stack_arena.
    //Allocate memory for container by using .push_back() which automatically calls function name allocator in ShortAlloc so: 
    cout<<"Before allocating vector object - memory of ShortAllocArena used: "<<stack_arena.used()<<endl;
    SmallVecObj.push_back(12);
    //so 4 bytes per int, some space is used to store address to return/exit subroutine

    cout<<"After allocating vector object - memory of ShortAlloc used: "<<stack_arena.used()<<endl;

    return 0;
}

//Go over difference between default constructors and constructor: className(); 
