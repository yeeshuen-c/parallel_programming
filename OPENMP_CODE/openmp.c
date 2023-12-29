#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// Define constants for the maximum size of the array, file name, and target value
#define MAX_SIZE 100
#define FILENAME "number.txt"
#define TARGET_VALUE 77

// Declare a global array of integers with maximum size MAX_SIZE
int arr[MAX_SIZE];

// Function implementing interpolation search in parallel using OpenMP
int openmpInterpolationSearch(int arr[], int l, int r, int key)
{
    // Perform interpolation search algorithm within the given array bounds [l, r]
    while (l <= r && key >= arr[l] && key <= arr[r])
    {
        // If the search range narrows down to a single element, check if it matches the key
        if (l == r)
        {
            if (arr[l] == key)
                return l; // Return index if the key is found
            return -1;    // Return -1 indicating key not found
        }

        int pos;

        // OpenMP parallel section for calculating the midpoint position 'pos' in parallel
#pragma omp parallel
        {
#pragma omp single nowait
            {
                // Calculate the midpoint position using interpolation formula
                pos = l + (((double)(r - l) / (arr[r] - arr[l])) * (key - arr[l]));
            }
        }

        // Check if the key is found at the calculated position 'pos'
        if (arr[pos] == key)
            return pos; // Return the index if the key is found

        // Narrow down the search range based on comparison with the calculated position
        if (arr[pos] < key)
            l = pos + 1; // Update left boundary
        else
            r = pos - 1; // Update right boundary
    }

    return -1; // Return -1 indicating key not found
}

// Main function taking command-line arguments
int main(int argc, char *argv[])
{
    int nos = MAX_SIZE;
    int nthread = atoi(argv[1]); // Get number of threads from command-line argument

    FILE *file;

    // Open the file for reading
    file = fopen(FILENAME, "r");
    if (file == NULL)
    {
        perror("Error opening file"); // Print error message if file opening fails
        return EXIT_FAILURE;          // Exit with failure status
    }

    // Read integers from the file into the array 'arr'
    for (int i = 0; i < MAX_SIZE; i++)
    {
        fscanf(file, "%d", &arr[i]);
    }

    // Set the number of threads for OpenMP parallel execution
    omp_set_num_threads(nthread);

    // Perform parallel interpolation search
    int result = openmpInterpolationSearch(arr, 0, nos - 1, TARGET_VALUE);

    // Print the result of the OpenMP search
    printf("\nOpenMP search result: target %d found at index %d\n", TARGET_VALUE, result);

    return 0; // Return with success status
}
