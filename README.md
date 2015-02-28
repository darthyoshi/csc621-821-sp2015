# visualize


#### ITK/VTK Crash Course

ITK and VTK are immensely powerful toolkits but they have a few API quirks that
can seem unintuitive and unfamiliar. This list of common hang-ups is maintained in 
order to help each independent team circumnavigate these obstacles.

*   Most ITK/VTK objects are merely interfaces or views into memory. They are
    not explicitly allocated on construction, as many people are used to from
    C++ code. For instance, when creating an image:

    ```cpp
    itk::Image<unsigned char, 2>::Pointer image = ImageType::New();
    ```
    Notice that we don't actually receive a reference to the object here but
    a pointer to a newly allocated image object. ITK/VTK internally use
    a custom "smart pointer" implementation that handles reference counting and
    automatic deallocation. At this point, all we've created is a data object
    pointer to a specific object of type `Image<unsigned char, 2>`. 

    Later, we configure the dimensions (and other properties) of our image
    memory object and finally call `image->Allocate()` which actually
    materializes our object in memory.

*   ITK/VTK's smart pointers are __NOT__ thread safe. It's a strange
    peculiarity of their custom smart pointer implementation and it differs
    from the standard C++ `std::smart_pointer` and competitive implementations
    in that reference counting is not atomic. ITK/VTK objects are therefore not
    safe to transfer beyond thread bounds. This shouldn't really matter to us,
    but, just in case, its worth pointing out.


#### Process Flow

To keep track of our general processing pipeline and rationale, we should
maintain this list of procedural steps from data loading to final visualization
and rendering. Modification to this list should be merged with the pull request
that implements the procedure and should close an associated GitHub issue. 

1.  Image Loading
2.  Registration
3.  Segmentation
4.  Visualization

#### C++11 Support

ITK/VTK make extensive use of templates and ridiculously long typenames.
Seriously, some fully qualified types will fill up an entire line. Rather than
suffer the anguish of having to constantly `typedef` everything over and over,
we can make use of the C++11 standard's `auto` keyword that allows the compiler
to "fill in the blank" for what type we're using in certain cases. For
instance:

```cpp
UnnecessarilyLongTypeName::WithAMysteriousTemplateParameter<3>
x = getThing();

// becomes...

auto x = getThing();
```

It's beautiful. However, it requires that you compile ITK with some specific
options turned on. Assuming you've downloaded the [ITK
source](http://www.itk.org/ITK/resources/software.html) to a directory named
`~/ITK`:

```
mkdir ITK-build && cd ITK-build
cmake -DCMAKE_CXX_FLAGS=-std=c++11 -DVCL_INCLUDE_CXX_0X:BOOL=ON ../ITK
make && make install
```

If you experience any issues with this process, try a completely clean build by
just recreating the build directory and trying again. If that doesn't solve it,
just post an issue.

#### Building

CMake advocates against in-source builds, meaning you should build the project
from another directory so as not to pollute the source directory and risk
committing extraneous files. To do this, simply follow these steps, starting
from the directory where this repository was cloned (~/imaging for this
example):

1.  `mkdir ../imaging-build`
2.  `cd ../imaging-build`
3.  `cmake ../imaging && make`

This creates a separate build directory outside the project, runs CMake from
that directory to generate the necessary makefiles, then compiles the actual
source and outputs the binaries.
