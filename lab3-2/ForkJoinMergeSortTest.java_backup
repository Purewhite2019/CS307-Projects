import java.util.concurrent.*;

public class ForkJoinMergeSortTest extends RecursiveAction {
    static final int DEFAULT_SIZE = 128;
    static final int RANGE = 16;

    public ForkJoinMergeSortTest() {}

    public static void main(String[] args) {
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
        pool.invoke(new ForkJoinMergeSortTask<Integer>(0, size-1, array, size, new Comparator<Integer>));

        for(int i = 0; i < size; ++i){
            System.out.print(array[i] + " ");
        }
        System.out.print('\n');
    }
}