#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <cstdint>
#include <climits>

#include <mcpdis.hh>

const range range::star(0,UINT_MAX);
const range range::plus(1,UINT_MAX);
const range range::qm(0,1);
const range range::one(1,1);
const range range::zero(0,0);
