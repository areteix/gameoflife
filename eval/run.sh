#!/bin/bash
GEN=10
MGEN=100
IN_FILE=in_file4
for j in `seq 1 10`;
do
    echo $j
    OUT_FILE=mm_r
    for i in `seq 1 8`;
    do
        ./life_matrix_malloc $i $IN_FILE 0 $MGEN >> $OUT_FILE
    done

    OUT_FILE=mm_c
    for i in `seq 1 8`;
    do
        ./life_matrix_malloc $i $IN_FILE 1 $MGEN >> $OUT_FILE
    done

    echo "TCMalloc"

    OUT_FILE=mt_r
    for i in `seq 1 8`;
    do
        ./life_matrix_tcmalloc $i $IN_FILE 0 $MGEN >> $OUT_FILE
    done

    OUT_FILE=mt_c
    for i in `seq 1 8`;
    do
        ./life_matrix_tcmalloc $i $IN_FILE 1 $MGEN >> $OUT_FILE
    done

    echo "life points..."

    OUT_FILE=pm_u
    for i in `seq 1 8`;
    do
        ./life_points_malloc $i $IN_FILE 0 $GEN >> $OUT_FILE
    done

    OUT_FILE=pm_m
    for i in `seq 1 8`;
    do
        ./life_points_malloc $i $IN_FILE 1 $GEN >> $OUT_FILE
    done

    echo "TCMalloc"
    OUT_FILE=pt_u
    for i in `seq 1 8`;
    do
        ./life_points_tcmalloc $i $IN_FILE 0 $GEN >> $OUT_FILE
    done

    OUT_FILE=pt_m
    for i in `seq 1 8`;
    do
        ./life_points_tcmalloc $i $IN_FILE 1 $GEN >> $OUT_FILE
    done
done
