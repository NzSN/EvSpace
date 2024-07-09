#ifndef EVSPACE_BASE_MACROS_H_
#define EVSPACE_BASE_MACROS_H_

#define FST(first, second) first
#define SND(first, second) second

#define TOP(first, ...) first
#define TAIL(first, ...) __VA_ARGS__

#endif /* EVSPACE_BASE_MACROS_H_ */
