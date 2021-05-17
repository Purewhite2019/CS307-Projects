import java.util.concurrent.*;
import java.lang.Comparable;

public class ForkJoinMergeSortTask extends RecursiveAction {
    static final int THRESHOLD = 64;
    private final int size;

    private int begin;
    private int end;
    private Comparable<Integer>[] array;

    public ForkJoinMergeSortTask(int begin, int end, Comparable<Integer>[] array, int size) {
        this.begin = begin;
        this.end = end;
        this.array = array;
        this.size = size;
    }

    protected void compute() {
        if(begin >= end)
            return;
        if(begin - end < THRESHOLD){
            boolean isFinished = false;
            T tmp;
            while(!isFinished){
                isFinished = true;
                for(int i = begin; i < end; ++i){
                    if(array[i].compareTo(array[i+1]) > 0){
                        tmp = array[i];
                        array[i] = array[i+1];
                        array[i+1] = tmp;
                        isFinished = false;
                    }
                }
            }
            return;
        }
        else{
            int mid = (begin+end)/2, i = begin, j = mid+1, index;
            Comparable<Integer> temp;
            
            ForkJoinMergeSortTask leftTask = new ForkJoinMergeSortTask(begin, mid, array, size);
            ForkJoinMergeSortTask rightTask = new ForkJoinMergeSortTask(mid+1, end, array, size);
            
            leftTask.fork();
            rightTask.fork();
            leftTask.join();
            rightTask.join();

            if(array[mid].compareTo(array[j]) <= 0)
                return;

            while(i <= mid && j <= end){
                if(array[i].compareTo(array[j]) <= 0)
                    ++i;
                else{
                    temp = array[j];
                    index = j;
                    while(index != i){
                        array[index] = array[index-1];
                        --index;
                    }
                    array[i] = temp;
                    
                    ++i; ++mid; ++j;
                }
            }
        }
    }
}