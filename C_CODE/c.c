#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define constants for the maximum size of the array, file name, and target value
#define MAX_SIZE 100          // Define your MAX_SIZE
#define FILENAME "number.txt" // Define your FILENAME
#define TARGET_VALUE 77       // Define your TARGET_VALUE

// Declare a global array of integers with maximum size MAX_SIZE
int arr[MAX_SIZE];
int result = -1;      // Variable to store the result of the search
pthread_mutex_t lock; // Mutex lock for thread synchronization

// Structure to hold thread arguments for interpolation search
struct ThreadArgs
{
    int *arr; // Pointer to the array
    int l;    // Left boundary for search range
    int r;    // Right boundary for search range
    int key;  // Key to be searched
};

// Function for performing interpolation search within a thread
void *InterpolationSearchThread(void *args)
{
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;

    // Extract search boundaries and key from the thread arguments
    int l = threadArgs->l;
    int r = threadArgs->r;
    int key = threadArgs->key;

    // Perform interpolation search within the specified range [l, r]
    while (l <= r && key >= threadArgs->arr[l] && key <= threadArgs->arr[r])
    {
        // If the search range narrows down to a single element, check if it matches the key
        if (l == r)
        {
            if (threadArgs->arr[l] == key)
            {
                // Acquire the lock and update the result if the key is found
                pthread_mutex_lock(&lock);
                result = l;
                pthread_mutex_unlock(&lock);
            }
            break;
        }

        // Calculate the position using interpolation formula
        int pos = l + (((double)(r - l) / (threadArgs->arr[r] - threadArgs->arr[l])) * (key - threadArgs->arr[l]));

        // Check if the key is found at the calculated position
        if (threadArgs->arr[pos] == key)
        {
            // Acquire the lock and update the result if the key is found
            pthread_mutex_lock(&lock);
            result = pos;
            pthread_mutex_unlock(&lock);
            break;
        }

        // Narrow down the search range based on comparison with the calculated position
        if (threadArgs->arr[pos] < key)
            l = pos + 1; // Update left boundary
        else
            r = pos - 1; // Update right boundary
    }

    return NULL;
}

// Main function taking command-line arguments
int main(int argc, char *argv[])
{
    // Check if the number of command-line arguments is adequate
    if (argc < 2)
    {
        printf("Usage: %s <number_of_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int nos = MAX_SIZE;
    int nthreads = atoi(argv[1]); // Get the number of threads from command-line argument

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

    // Create arrays to hold thread IDs and thread arguments
    pthread_t threads[nthreads];
    struct ThreadArgs threadArgs[nthreads];

    // Initialize the mutex lock for synchronization
    pthread_mutex_init(&lock, NULL);

    // Divide the array into segments for each thread and create threads for parallel search
    for (int i = 0; i < nthreads; i++)
    {
        int chunk_size = MAX_SIZE / nthreads;
        int start = i * chunk_size;
        int end = (i == nthreads - 1) ? (MAX_SIZE - 1) : (start + chunk_size - 1);

        // Set the thread arguments for each segment
        threadArgs[i].arr = arr;
        threadArgs[i].l = start;
        threadArgs[i].r = end;
        threadArgs[i].key = TARGET_VALUE;

        // Create threads and pass the InterpolationSearchThread function with thread arguments
        pthread_create(&threads[i], NULL, InterpolationSearchThread, (void *)&threadArgs[i]);
    }

    // Wait for all threads to finish execution
    for (int i = 0; i < nthreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Destroy the mutex lock after thread execution is complete
    pthread_mutex_destroy(&lock);

    // Print the result of the pthread search
    printf("\nC(pthread) result: target %d found at index %d\n", TARGET_VALUE, result);

    return 0; // Return with success status
}
