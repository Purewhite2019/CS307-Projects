import java.util.concurrent.*;

public class ForkJoinMergeSortTask extends RecursiveAction {
    static final int THRESHOLD = 64;
    static final int DEFAULT_SIZE = 128;
    static final int RANGE = 16;
    private final int size;

    private int begin;
    private int end;
    private int[] array;

    public ForkJoinMergeSortTask(int begin, int end, int[] array, int size) {
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
            int tmp;
            while(!isFinished){
                isFinished = true;
                for(int i = begin; i < end; ++i){
                    if(array[i] > array[i+1]){
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
            int mid = (begin+end)/2, i = begin, j = mid, k = 0, tmp;
            int[] temp = new int[end-begin+1];
            
            ForkJoinMergeSortTask leftTask = new ForkJoinMergeSortTask(begin, mid, array, size);
            ForkJoinMergeSortTask rightTask = new ForkJoinMergeSortTask(mid+1, end, array, size);
            
            leftTask.fork();
            rightTask.fork();
            leftTask.join();
            rightTask.join();

            while(i < mid && j < end){
                if(array[i] < array[j])
                    temp[k++] = array[i++];
                else
                    temp[k++] = array[j++];
            }
            while(i < mid){
                temp[k++] = array[i++];
            }
            while(j < end){
                temp[k++] = array[j++];
            }
            for(int t = 0; t < k; ++t)
                array[begin+t] = temp[t];
            return;
        }
    }

    public static void main(String[] args) throws Exception{
        ForkJoinPool pool = new ForkJoinPool();
        int size = args.length;
        int[] array;
        if(size == 0){
            java.util.Random rand = new java.util.Random();
            size = DEFAULT_SIZE;
            array = new int[size];
            for(int i = 0; i < size; ++i)
                array[i] = rand.nextInt(RANGE);
            System.out.println("Randomly generated " + size + " elements in range " + RANGE + ".");
            for(int i = 0; i < size; ++i){
                System.out.print(array[i] + " ");
            }
            System.out.print('\n');
        }
        else{
            array = new int[size];
            for(int i = 0; i < size; ++i)
                array[i] = Integer.parseInt(args[i]);
        }
        pool.invoke(new ForkJoinMergeSortTask(0, size-1, array, size));

        for(int i = 0; i < size; ++i){
            System.out.print(array[i] + " ");
        }
        System.out.print('\n');
    }
}