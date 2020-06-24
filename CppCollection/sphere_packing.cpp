

/********************************************************
*	Implementation of the Sphere Packing classes	*
*********************************************************/

/*#include <float.h>
#include "base/binary.h"
#include "base/matfunc.h"
#include "base/specmat.h"
#include "comm/modulator.h"
#include "comm/commfunc.h"
#include <complex>
#include "def.h"*/
#include "sphere_packing.h"

using namespace::itpp;
using namespace::std;

/*--------------------------------------------------------------------
	------------------- Class: SPHERE_PACKING -------------------
	--------------------------------------------------------------------*/

/*
** SPHERE_PACKING::SPHERE_PACKING(int in_dimension, int in_L, int in_mapping, int in_nrof_Tx, int in_nrof_Rx)
**
**		Constructor that initializes the class with predefined parameters.
**
**	Parameters:
**                 in_dimension     ==>     dimension of sphere packing only 4 or 6 so far
**                 in_L             ==>     L "the length of the sphere packing set"
**
**	Returns: None
**
**	Notes: N/A
*/
SPHERE_PACKING::SPHERE_PACKING ( int in_dimension, int in_L, int in_mapping, int in_nrof_Tx, int in_nrof_Rx )
{


    // testing
    /*
    double a=-1000;
    double b=-2;

    double exact_v = log(exp(a)+exp(b));
    double log_v = jacolog(a,b);
    double log_approx_v = jacolog_approx(a,b);
    double max_log_v = max(a,b);

    cout << "exact = "<< exact_v << endl;
    cout << "log = "<< log_v << endl;
    cout << "log-approx = "<< log_approx_v << endl;
    cout << "max-log = "<< max_log_v << endl;

    exit(0);
    */
    // end testing //



    //checking the dimension
    if ( in_dimension == 4 )
    {
        dimension = in_dimension;
    }
    else if ( in_dimension == 6 )
    {
        it_error ( "\n\nSorry: Sphere Packing of dimension 6 has not been implemented yet\n" );
    }
    else
    {
        it_error ( "dimension must be 4 or 6 only !!" );
    }


    //checking  L

    L = in_L;
    if ( round_i ( pow2 ( round_i ( ( log2 ( L ) ) ) )-pow2 ( log2 ( L ) ) ) != 0 )
    {
        cout << endl;
        cout <<"ERROR: L = "<< L<<" , ";
        cout <<"The nearest value is "<< round_i ( pow2 ( round_i ( ( log2 ( L ) ) ) ) ) <<endl;
        cout << endl;
        it_error ( "\n\nERROR: Log2(L) is not an integer\n" );
    }

    //checking the mapping
    //if (in_mapping < 0) {
    //it_error("\n\nMapping must be > 0\n");
    //}else if (in_mapping > 4) {
    //it_error("\n\nMapping must be <= 4\n");
    //}

    mapping = in_mapping;

    //setting the number of transmit and receive antennas
    nrof_Tx = in_nrof_Tx;
    nrof_Rx = in_nrof_Rx;

    //initialising iterations counter
    iterations_counter = 0;

    //setting block_size
    block_size = round_i ( log2 ( L ) );

    //setting the size of sphere_packing_matrix
    sphere_packing_matrix.set_size ( L,dimension,false );

    //setting the size of original_matrix -- used so that a random is always campared to the same matrix
    original_matrix.set_size ( L,dimension,false );

    //setting the size of mapping_state_index ivec;
    mapping_state_index.set_size ( L,false );

    //calling the appropriate function to create the sphere_packing_matrix
    switch ( dimension )
    {

    case D4:

        nrof_symbs_per_block = D4_nrof_symbs_per_block;

        //create the sphere packing D4 matrix as in page 45 log book 1
        sphere_packing_matrix = create_sphere_packing_D4_matrix();
        break;

    case D6:

        nrof_symbs_per_block = D6_nrof_symbs_per_block;

        //create the sphere packing D6 matrix as in page ..? log book 1
        it_error ( "\n\nERROR: not implemented yet\n" );
        sphere_packing_matrix = create_sphere_packing_D6_matrix();
        break;

    default:

        it_error ( "\n\nERROR: invalid dimension\n" );
        break;
    }

    //setting up the original_matirx
    original_matrix = sphere_packing_matrix;

    //calculating the notmalization factor sqrt(2L/E)
    norm_factor = calculate_norm_factor();

    //create the modulation symbols List (ex D4: x1 and x2) as in page 46 of log book 1
    modulation_symbs_list = create_modulation_symbs_list();


}

/*
**	SPHERE_PACKING::~SPHERE_PACKING()
**
**		Destructor that releases the resources occupied by the program.
**
**	Parameters: N/A
**
**	Returns: None
**
**	Notes: N/A
*/
SPHERE_PACKING::~SPHERE_PACKING()
{
    //Release occupied resources here.

    // free(sphere_packing_matrix);

    //free(bitmap);

    //free(S0,S1);

    //free(modulation_symbs_list);
}





void SPHERE_PACKING::get_new_random_mapping ( FILE *outputfile )
{

    int i,j,n,tmp,current_index=0;
    bool chosen=false;

    ivec integer_index ( L );


    // this is the old way to randomly permute integer_index

    //initialise integer_index
    //for (i=0; i<L; i++) { integer_index(i) = i; }

    //for (n=0; n<11; n++)
    //{
    //  for (i=0; i<L; i++)
    //{
    //  j = randi(0,L-1);
    //  tmp = integer_index(j);
    //  integer_index(j) = integer_index(i);
    //  integer_index(i) = tmp;
    //}
    //}


    //this is my way to create a random integer_index each time

    //initialise integer_index
    for ( i=0; i<L; i++ )
    {
        integer_index ( i ) = 100;
    }

    for ( n=0; n<10000; n++ )
    {
        j = randi ( 0,L-1 );
        //check if j was already chosen
        chosen = false;
        for ( i=0; i<L; i++ )
        {
            if ( integer_index ( i ) == j ) chosen = true;
        }

        if ( !chosen )
        {
            integer_index ( current_index ) = j;
            current_index++;
        }
        if ( current_index == L ) break;
    }


    //this is another way to produce a controlled new integer_index

    //initialise integer_index
    //for (i=0; i<L; i++) { integer_index(i) = i; }
    //
    //switch(random_index) {
    //case (0):
    //  //swap 0 and 1
    //integer_index(1) = 2;
    //integer_index(2) = 1;
    //break;
    //
    //case(1):
    ////swap 0 and 3
    //integer_index(3) = 5;
    //integer_index(5) = 3;
    //break;
    //
    //case(2):
    ////swap 0 and 7
    //integer_index(7) = 14;
    //integer_index(14) = 7;
    //break;

    //case(3):
    ////swap 0 and 15
    //integer_index(8) = 12;
    //integer_index(12) = 8;
    //break;
    //}

    //Display the new random mapping
    printf ( "Current index mapping as compared to AGM2 mapping: \n" );
    fprintf ( outputfile,"Current index mapping as compared to AGM2 mapping: \n" );
    for ( i=0; i<L; i++ )
    {
        printf ( "%d - ",integer_index ( i ) );
        fprintf ( outputfile,"%d - ",integer_index ( i ) );
    }
    printf ( "\n" );
    fprintf ( outputfile,"\n" );

    //update the random_matrix
    for ( i=0; i<L; i++ )
    {
        sphere_packing_matrix.set_row ( i,original_matrix.get_row ( integer_index ( i ) ) );
    }

    //create the modulation symbols List (ex D4: x1 and x2) as in page 46 of log book 1
    modulation_symbs_list = create_modulation_symbs_list();

}


// private
// create the sphere packing D4 matrix as in page 45 log book 1
imat SPHERE_PACKING::create_sphere_packing_D4_matrix ( void )
{
    imat matrix ( L,dimension );
    int i,j,a,b,c,d,nRows,nColumns,index=0,exp1,exp2;
    int k,count0,count1;
    bvec bits;
    bool matrix_done=false;


    // generating the sign matrix
    imat sign ( 16,4 );               // an integer matrix whois elements are only +1 or -1
    bvec binary_number ( 4 );         // a bvec that will hold the 0's and 1's of a four-digit binary number


    for ( i=0 ; i<16 ; i++ )
    {
        binary_number = dec2bin ( 4,i );
        for ( j=0 ; j<4 ; j++ )
        {
            sign ( i,j ) = 2* ( int ) binary_number ( j ) - 1;
        }
    }


    matrix.clear();

    switch ( L )
    {

    case 4:
        // start of 4 states //

        switch ( mapping )
        {

        case ( 0 ) :

            // Gray mapping

            // 00
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            index++;

            // 01
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            index++;

            // 10
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            index++;

            // 11
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            break;

        case ( 1 ) :

            //Set-partitioning

            //00
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;

            //01
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;

            //10
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;

            //11
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            break;

        default:

            cout <<endl <<endl << "ERROR - in sphere packing initialising " <<endl;
            cout << " Mapping " << mapping << " is not defined for 4-state sphere packing"<< endl<<endl<<endl;
            exit ( -1 );
        }

        //  end of 4 states  //
        break;



    case 8:
        // start of 8 states //
        matrix ( index,0 ) = -1;
        matrix ( index,1 ) = -1;
        matrix ( index,2 ) = 0;
        matrix ( index,3 ) = 0;
        index++;

        matrix ( index,0 ) = -1;
        matrix ( index,1 ) = 1;
        matrix ( index,2 ) = 0;
        matrix ( index,3 ) = 0;
        index++;

        matrix ( index,0 ) = 1;
        matrix ( index,1 ) = -1;
        matrix ( index,2 ) = 0;
        matrix ( index,3 ) = 0;
        index++;

        matrix ( index,0 ) = 1;
        matrix ( index,1 ) = 1;
        matrix ( index,2 ) = 0;
        matrix ( index,3 ) = 0;
        index++;

        matrix ( index,0 ) = 0;
        matrix ( index,1 ) = 0;
        matrix ( index,2 ) = -1;
        matrix ( index,3 ) = -1;
        index++;

        matrix ( index,0 ) = 0;
        matrix ( index,1 ) = 0;
        matrix ( index,2 ) = -1;
        matrix ( index,3 ) = 1;
        index++;

        matrix ( index,0 ) = 0;
        matrix ( index,1 ) = 0;
        matrix ( index,2 ) = 1;
        matrix ( index,3 ) = -1;
        index++;

        matrix ( index,0 ) = 0;
        matrix ( index,1 ) = 0;
        matrix ( index,2 ) = 1;
        matrix ( index,3 ) = 1;
        // end of 8 states //
        break;


    case 16:

        // start of 16 states //

        switch ( mapping )
        {

        case ( 0 ) :

            // Gray mapping
            Io = 0.6421;
            Io_dB = 2.0;

            // maps to bin(0)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;

            // maps to bin(1)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(2)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(3)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(4)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(5)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(6)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(7)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(8)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12 ;
            index++;

            // maps to bin(9)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(10)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(11)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) =0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(12)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(13)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(14)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(15)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;

            break;

        case ( 1 ) :

            //AGM distance 3

            Io = 0.2840;
            Io_dB = 2.0;

            // maps to bin(0)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;


            // maps to bin(1)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(2)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(3)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(4)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(5)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(6)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(7)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(8)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(9)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(10)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(11)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(12)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(13)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12;
            index++;

            // maps to bin(14)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(15)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;

            break;

        case ( 2 ) :

            //Random AGM

            Io = 0.4276;
            Io_dB = 2.0;

            // maps to bin(0)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;


            // maps to bin(1)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(2)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(3)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;
            index++;

            // maps to bin(4)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(5)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(6)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(7)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(8)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(9)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(10)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(11)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(12)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12;
            index++;

            // maps to bin(13)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(14)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(15)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;

            break;

        case ( 3 ) :

            // Set Partioning

            Io = 0.3968;
            Io_dB = 2.0;

            // maps to bin(0)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;


            // maps to bin(1)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;

            // maps to bin(2)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12;
            index++;

            // maps to bin(3)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(4)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(5)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(6)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(7)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(8)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(9)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;
            index++;

            // maps to bin(10)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(11)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(12)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(13)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(14)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(15)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;

            break;

        case ( 4 ) :

            //a randomly permuted AGM

            Io = 0.4527;
            Io_dB = 2.0;

            // maps to bin(0)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;


            // maps to bin(1)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;


            // maps to bin(2)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;



            // maps to bin(3)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;


            // maps to bin(4)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;


            // maps to bin(5)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;


            // maps to bin(6)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;
            index++;

            // maps to bin(7)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;


            // maps to bin(8)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;


            // maps to bin(9)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12;
            index++;


            // maps to bin(10)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;



            // maps to bin(11)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;


            // maps to bin(12)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;


            // maps to bin(13)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(14)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(15)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;

            break;

        case ( 5 ) :

            // A randomly permuted mapping
            Io = 0.5476;
            Io_dB = 2.0;

            // maps to bin(0)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;
            index++;

            // maps to bin(1)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(2)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(3)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(4)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(5)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(6)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(7)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(8)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12 ;
            index++;

            // maps to bin(9)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(10)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(11)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) =0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(12)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(13)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(14)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(15)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;


            break;

        case ( 6 ) :

            // a randomly permuted mapping
            Io = 0.5901;
            Io_dB = 2.0;

            // maps to bin(0)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(1)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(2)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(3)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;


            // maps to bin(4)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(5)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(6)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(7)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(8)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12 ;
            index++;

            // maps to bin(9)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(10)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(11)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) =0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(12)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(13)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(14)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(15)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;

            break;

        case ( 7 ) :

            // a randomly permuted mapping
            Io = 0.3681;
            Io_dB = 2.0;

            // maps to bin(0)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;
            index++;


            // maps to bin(1)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(2)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(3)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(4)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(5)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(6)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(7)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(8)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(9)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(10)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(11)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(12)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(13)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12;
            index++;

            // maps to bin(14)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(15)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;

            break;

        case ( 8 ) :

            // a randomly permuted mapping
            Io = 0.5003;
            Io_dB = 2.0;
            //Io = 0.6870 @ 4dB


            // maps to bin(0)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(1)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(2)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(3)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12 ;
            index++;

            // maps to bin(4)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(5)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;
            index++;

            // maps to bin(6)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(7)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(8)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(9)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(10)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(11)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;

            // maps to bin(12)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(13)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) =0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(14)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(15)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;

            break;

        case ( 9 ) :

            // a randomly permuted mapping
            Io = 0.4751;
            Io_dB = 2.0;
            //Io = 0.6681 @ 4dB

            // maps to bin(0)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(1)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(2)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12 ;
            index++;

            // maps to bin(3)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(4)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(5)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;
            index++;

            // maps to bin(6)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(7)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) =0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(8)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(9)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;

            // maps to bin(10)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(11)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(12)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(13)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(14)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(15)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;

            break;

        case ( 10 ) :

            // a randomly permuted mapping
            Io = 0.5193;
            Io_dB = 2.0;
            //Io = 0.7028 @ 4dB

            // maps to bin(0)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) =0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(1)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(2)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12 ;
            index++;

            // maps to bin(3)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(4)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(5)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(6)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(7)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(8)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(9)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(10)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;

            // maps to bin(11)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;
            index++;

            // maps to bin(12)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(13)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(14)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;
            index++;

            // maps to bin(15)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;

            break;

        case ( 11 ) :

            // a randomly permuted mapping
            Io = 0.3485;
            Io_dB = 2.0;
            //Io =  0.5679@ 4dB

            // maps to bin(0)
            // state 11
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 11;
            index++;

            // maps to bin(1)
            // state 0
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 0;
            index++;

            // maps to bin(2)
            // state 1
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 1;
            index++;

            // maps to bin(3)
            // state 14
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) =0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 14;
            index++;

            // maps to bin(4)
            // state 5
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 5;
            index++;

            // maps to bin(5)
            // state 8
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 8;
            index++;

            // maps to bin(6)
            // state 12
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 12 ;
            index++;

            // maps to bin(7)
            // state 15
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 15;
            index++;

            // maps to bin(8)
            // state 2
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 2;
            index++;

            // maps to bin(9)
            // state 9
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 9;
            index++;

            // maps to bin(10)
            // state 10
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = -1;
            mapping_state_index ( index ) = 10;
            index++;

            // maps to bin(11)
            // state 6
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 6;
            index++;

            // maps to bin(12)
            // state 3
            matrix ( index,0 ) = 1;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 3;
            index++;

            // maps to bin(13)
            // state 7
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = 1;
            matrix ( index,2 ) = 1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 7;
            index++;

            // maps to bin(14)
            // state 4
            matrix ( index,0 ) = 0;
            matrix ( index,1 ) = -1;
            matrix ( index,2 ) = -1;
            matrix ( index,3 ) = 0;
            mapping_state_index ( index ) = 4;
            index++;

            // maps to bin(15)
            // state 13
            matrix ( index,0 ) = -1;
            matrix ( index,1 ) = 0;
            matrix ( index,2 ) = 0;
            matrix ( index,3 ) = 1;
            mapping_state_index ( index ) = 13;

            break;
        }

        // end of 16 states //

    }

    // For states >= 32 pls go to "sphere_packing_mapping.cpp"

    // Calculate the bit pattern "bitmap"
    bitmap.set_size ( L,block_size,false );

    for ( i=0; i<L; i++ )
    {
        bitmap.set_row ( i,dec2bin ( block_size,i ) );
    }



    // Calculate the soft bit mapping matrices S0 and S1
    S0.set_size ( block_size,L/2,false );
    S1.set_size ( block_size,L/2,false );

    for ( k=0; k<block_size; k++ )
    {
        count0 = 0;
        count1 = 0;
        for ( index=0; index<L; index++ )
        {
            bits = bitmap.get_row ( index );

            if ( bits ( ( block_size-1 )-k ) ==bin ( 0 ) )
            {
                S0 ( k,count0 ) = index;
                count0++;
            }
            else
            {

                S1 ( k,count1 ) = index;
                count1++;
            }
        }
    }

    return ( matrix );
}



imat SPHERE_PACKING::create_sphere_packing_D6_matrix ( void )
{
    it_error ( "\n\nERROR: not yet\n" );

}



// private
// create the modulation symbols list x1,x2 as in page 46 of log book 1
//
//  Important: the symbols are normalised here
//
cvec SPHERE_PACKING::create_modulation_symbs_list ( void )
{
    int i,j;
    cvec symbols;


    symbols.set_size ( nrof_symbs_per_block*L,false );

    for ( i=0 ; i<L ; i++ )
    {
        for ( j=0; j<nrof_symbs_per_block; j++ )
        {
            symbols ( 2*i+j ) = norm_factor * double_complex ( sphere_packing_matrix ( i,2*j ),sphere_packing_matrix ( i,2*j+1 ) );
        }
    }

    return ( symbols );
}



void SPHERE_PACKING::display_parameters ( FILE *in_outfile )
{
    int i,j;
    bvec binary_block;

    binary_block.set_size ( block_size,false );



    //printf("\n!Sphere Packing Parameters:\n");
    fprintf ( in_outfile, "\n!Sphere Packing Parameters:\n" );

    //printf("!-------------------------\n");
    fprintf ( in_outfile, "!-------------------------\n" );

    //printf("\n!   Sphere Packing Dimension = %d\n",dimension);
    fprintf ( in_outfile, "\n!   Sphere Packing Dimension = %d\n",dimension );

    //printf("\n!   Sphere Packing Size = %d constellation points\n",L);
    fprintf ( in_outfile, "\n!   Sphere Packing Size = %d constellation points\n",L );

    if ( L == 16 )
    {
        switch ( mapping )
        {

        case ( 0 ) :

            //Gray mapping
            //printf("\n!   Mapping : %d, Gray Mapping\n",mapping);
            fprintf ( in_outfile,"\n!   Mapping : %d, Gray Mapping\n",mapping );
            break;

        case ( 1 ) :

            //AGM with distance 3
            //printf("\n!   Mapping : %d, Anti-Gray Mapping\n",mapping);
            fprintf ( in_outfile,"\n!   Mapping : %d, Anti-Gray Mapping\n",mapping );
            break;

        case ( 2 ) :

            //Random AGM -- the original mapping --
            //printf("\n!   Mapping : %d, Anti-Gray Mapping\n",mapping);
            fprintf ( in_outfile,"\n!   Mapping : %d, Anti-Gray Mapping\n",mapping );
            break;

        case ( 3 ) :

            //Set Partioning
            //printf("\n!   Mapping : %d, Set Partioning\n",mapping);
            fprintf ( in_outfile,"\n!   Mapping : %d, Set Partioning\n",mapping );
            break;

        default:

            //a randomly permuted AGM
            //printf("\n!   Mapping : %d, Anti-Gray Mapping\n",mapping);
            fprintf ( in_outfile,"\n!   Mapping : %d, Anti-Gray Mapping\n",mapping );
            break;

        }

        //display mapping_state_index as compared to the original random mapping
        //printf("\n!   Mapping State Index relative to the original random mapping (2):\n");
        fprintf ( in_outfile,"\n!   Mapping State Index relative to the original random mapping (2):\n" );
        //printf("\n!   ");
        fprintf ( in_outfile,"\n!   " );

        for ( i=0; i<L ; i++ )
        {
            //printf("%d - ",mapping_state_index(i));
            fprintf ( in_outfile,"%d - ",mapping_state_index ( i ) );
        }
        //printf("\n");
        fprintf ( in_outfile,"\n" );

        //disply mutual information of current mapping
        //printf("\n!   Bit-level mutual information: %6.4f @ %2.1fdB\n",Io,Io_dB);
        fprintf ( in_outfile, "\n!   Bit-level mutual information: %6.4f @ %2.1fdB\n",Io,Io_dB );

    }

    //printf("\n!   Sphere Packing Mapping List:\n");
    fprintf ( in_outfile, "\n!   Sphere Packing Mapping List:\n" );

    //printf("\n!index       ");
    fprintf ( in_outfile, "\n!index       " );

    for ( j=0; j<block_size; j++ )
    {
        //printf("b%d",block_size-j-1);
        fprintf ( in_outfile,"b%d",block_size-j-1 );
    }

    //printf("        (");
    fprintf ( in_outfile, "        (" );

    for ( j=0; j<dimension; j++ )
    {
        if ( j != dimension-1 )
        {
            //printf("a%d,",j);
            fprintf ( in_outfile,"a%d,",j );
        }
        else
        {
            //printf("a%d)\n",j);
            fprintf ( in_outfile,"a%d)\n",j );
        }
    }

    for ( i=0; i<L; i++ )
    {
        binary_block = dec2bin ( block_size,i );

        if ( i<10 )
        {
            //printf("! %d      -->   ",i);
            fprintf ( in_outfile,"! %d      -->   ",i );
        }
        else if ( i<100 )
        {
            //printf("! %d     -->   ",i);
            fprintf ( in_outfile,"! %d     -->   ",i );
        }
        else if ( i<1000 )
        {
            //printf("! %d    -->   ",i);
            fprintf ( in_outfile,"! %d    -->   ",i );
        }


        for ( j=0; j<block_size; j++ )
        {
            //printf("%d",(int)binary_block(j));
            fprintf ( in_outfile,"%d", ( int ) binary_block ( j ) );
        }

        //printf("    -->    (");
        fprintf ( in_outfile, "    -->    (" );

        for ( j=0; j<dimension; j++ )
        {
            if ( j != dimension-1 )
            {
                //printf("%d,",sphere_packing_matrix(i,j));
                fprintf ( in_outfile,"%d,",sphere_packing_matrix ( i,j ) );
            }
            else
            {
                //printf("%d)\n",sphere_packing_matrix(i,j));
                fprintf ( in_outfile,"%d)\n",sphere_packing_matrix ( i,j ) );
            }
        }
    }

    fflush ( in_outfile );
}


//display parameters to screen
void SPHERE_PACKING::display_parameters ( void )
{
    int i,j;
    bvec binary_block;

    binary_block.set_size ( block_size,false );

    //setting output_file
    //output_file = in_outfile;


    printf ( "\n!Sphere Packing Parameters:\n" );
    printf ( "!-------------------------\n" );
    printf ( "\n!   Sphere Packing Dimension = %d\n",dimension );
    printf ( "\n!   Sphere Packing Size = %d constellation points\n",L );

    if ( L == 16 )
    {
        switch ( mapping )
        {

        case ( 0 ) :

            //Gray mapping
            printf ( "\n!   Mapping : %d, Gray Mapping\n",mapping );
            break;

        case ( 1 ) :

            //AGM with distance 3
            printf ( "\n!   Mapping : %d, Anti-Gray Mapping\n",mapping );
            break;

        case ( 2 ) :

            //Random AGM -- the original mapping --
            printf ( "\n!   Mapping : %d, Anti-Gray Mapping\n",mapping );
            break;

        case ( 3 ) :

            //Set Partioning
            printf ( "\n!   Mapping : %d, Set Partioning\n",mapping );
            break;

        default:

            //a randomly permuted AGM
            printf ( "\n!   Mapping : %d, Anti-Gray Mapping\n",mapping );
            break;

        }

        //display mapping_state_index as compared to the original random mapping
        printf ( "\n!   Mapping State Index relative to the original random mapping (2):\n" );
        printf ( "\n!   " );

        for ( i=0; i<L ; i++ )
        {
            printf ( "%d - ",mapping_state_index ( i ) );
        }
        printf ( "\n" );


        //disply mutual information of current mapping
        printf ( "\n!   Bit-level mutual information: %6.4f @ %2.1fdB\n",Io,Io_dB );

    }

    printf ( "\n!   Sphere Packing Mapping List:\n" );
    printf ( "\n!index       " );

    for ( j=0; j<block_size; j++ )
    {
        printf ( "b%d",block_size-j-1 );

    }

    printf ( "        (" );


    for ( j=0; j<dimension; j++ )
    {
        if ( j != dimension-1 )
        {
            printf ( "a%d,",j );

        }
        else
        {
            printf ( "a%d)\n",j );
        }
    }

    for ( i=0; i<L; i++ )
    {
        binary_block = dec2bin ( block_size,i );

        if ( i<10 )
        {
            printf ( "! %d      -->   ",i );
        }
        else if ( i<100 )
        {
            printf ( "! %d     -->   ",i );
        }
        else if ( i<1000 )
        {
            printf ( "! %d    -->   ",i );
        }


        for ( j=0; j<block_size; j++ )
        {
            printf ( "%d", ( int ) binary_block ( j ) );
        }

        printf ( "    -->    (" );

        for ( j=0; j<dimension; j++ )
        {
            if ( j != dimension-1 )
            {
                printf ( "%d,",sphere_packing_matrix ( i,j ) );
            }
            else
            {
                printf ( "%d)\n",sphere_packing_matrix ( i,j ) );
            }
        }
    }
}



// private
// general for D4 and D6
void SPHERE_PACKING:: display_matrix ( const imat matrix,int rows,int cols )
{
    int i,j;

    fprintf ( output_file,"!Sphere Packing D%d ---- Sphere Packing D%d ---- Sphere Packing D%d\n\n",
              cols,cols,cols );


    fprintf ( output_file,"!L" );
    for ( i=0; i<cols; i++ )
    {
        fprintf ( output_file,"\tal,%d",i+1 );
    }
    fprintf ( output_file,"\n\n" );



    for ( i=0; i<rows; i++ )
    {
        fprintf ( output_file,"!%d",i );
        for ( j=0; j<cols; j++ )
        {
            fprintf ( output_file,"\t%d",matrix ( i,j ) );
        }
        fprintf ( output_file,"\n" );
    }
}


// private
// general D4 and D6
void SPHERE_PACKING::display_modulation_symbs_list ( const cvec &symbs_list )
{
    int i,j,nrof_blocks;
    bvec binaryBlockVec;
    ivec index_binary;

    fprintf ( output_file,"\n\n\n" );
    fprintf ( output_file,"!------------- The Normalized Symbol List -----------\n\n\n" );

    fprintf ( output_file,"!Block" );
    for ( i=0; i<nrof_symbs_per_block; i++ )
    {
        fprintf ( output_file,"\t\tX%d",i+1 );
    }
    fprintf ( output_file,"\n\n" );



    nrof_blocks = round_i ( pow ( 2,block_size ) ); //number of binary blocks of size "block_size"
    // round_i is to make it integer

    for ( i=0; i<nrof_blocks; i++ )
    {
        binaryBlockVec = dec2bin ( block_size,i );
        index_binary = to_ivec ( binaryBlockVec );
        fprintf ( output_file,"!" );
        for ( j=0; j<block_size; j++ )
        {
            fprintf ( output_file,"%d",index_binary ( j ) );

        }


        for ( j=0; j<nrof_symbs_per_block; j++ )
        {
            fprintf ( output_file,"\t%2fj%2f",symbs_list ( i*nrof_symbs_per_block+j ).real(),
                      symbs_list ( i*nrof_symbs_per_block+j ).imag() );
        }
        fprintf ( output_file,"\n" );
    }
}

// private
// calculating the notmalization factor sqrt(2L/E)
// general D4 and D6
double SPHERE_PACKING::calculate_norm_factor ( void )
{
    int i,j,rows,cols;
    double TotalEnergy=0;
    cols = dimension;
    rows = L;

    for ( i=0 ; i<rows ; i++ )
    {
        for ( j=0 ; j<cols ; j++ )
        {
            TotalEnergy += pow ( sphere_packing_matrix ( i,j ),2 );
        }
    }
    //cout << "Norm Factor1 = " << sqrt(2*L/TotalEnergy) << endl << endl;

    /*
    // only for testing
    int testor,momo;
    for (i=0 ; i<rows ; i++)
      {
        testor = 0;
        for (j=0 ; j<cols ; j++)
    {
      testor += sphere_packing_matrix(i,j);
    }
        cout << i+1 << ") " << sphere_packing_matrix(i,0) << " " << sphere_packing_matrix(i,1) << " " << sphere_packing_matrix(i,2) << " " << sphere_packing_matrix(i,3) << " " << " Total "<< testor <<endl;
        cin >> momo;
      }
    // end testing
    */

    return ( sqrt ( 2* ( double ) L/TotalEnergy ) );
}















/*
**	cvec SPHERE_PACKING::modulate_bits(const bvec &bits)
**
**
**
**	Parameters:
**                 bits     ==>     binary input bits
**
**	Returns:   modulated symbols
**
**	Notes: N/A
*/


void SPHERE_PACKING::modulate_bits ( const bvec &bits, cvec &modulated_symbols )
{

    bvec block;
    int i,j,nrof_blocks,index,nrof_symbs;

    nrof_blocks = floor_i ( bits.length() /block_size );


    nrof_symbs = nrof_blocks * nrof_symbs_per_block;


    modulated_symbols.set_size ( nrof_symbs,false );
    block.set_size ( block_size,false );    // defining the binary vector that will take b bits a time to
    // convert them to an integer index




    for ( i=0; i<nrof_blocks; i++ )
    {
        //Group bits in a binary block of size "block_size"
        for ( j=0; j<block_size; j++ )
        {
            block ( j ) = bits ( i*block_size+j );
        }

        index = bin2dec ( block,false );

        //note that normalisation is done already when creating modulation_symbs_list()
        //

        modulated_symbols ( 2*i ) = modulation_symbs_list ( 2*index );
        modulated_symbols ( 2*i+1 ) = modulation_symbs_list ( 2*index+1 );
    }
}

/*
**	void SPHERE_PACKING::modulate_bits(const bvec &bits, cmat &symbols)
**
**
**
**	Parameters:
**                 bits     ==>     binary input bits
**
**	Returns:   modulated symbols
**
**	Notes: N/A
*/

void SPHERE_PACKING::modulate_bits ( const bvec &bits, cmat &symbols )
{


    bvec block;
    int i,j,nrof_blocks,index,nrof_symbs;

    nrof_blocks = floor_i ( bits.length() /block_size );

    nrof_symbs = nrof_blocks * nrof_symbs_per_block;

    symbols.set_size ( nrof_blocks,nrof_symbs_per_block,false );
    block.set_size ( block_size,false );    // defining the binary vector that will take b bits a time to
    // convert them to an integer index


    for ( i=0; i<nrof_blocks; i++ )
    {
        //Group bits in a binary block of size "block_size"
        for ( j=0; j<block_size; j++ )
        {
            block ( j ) = bits ( i*block_size+j );
        }

        index = bin2dec ( block,false );
        //feed in the appropriate symbols
        symbols ( i,0 ) = modulation_symbs_list ( 2*index );
        symbols ( i,1 ) = modulation_symbs_list ( 2*index+1 );
    }
}

/*
**	cvec SPHERE_PACKING::modulate_symbols(const ivec &in_symbols)
**
**
**
**	Parameters:
**                 in_symbols     ==>     integer input symbols
**
**	Returns:   complex modulated symbols
**
**	Notes: use this when there is one-to-one mapping between input and output
*/

void SPHERE_PACKING::modulate_symbols ( const ivec &in_symbols, cvec &out_symbols )
{
    int i,index;

    out_symbols.set_size ( in_symbols.length() *nrof_symbs_per_block,false );

    for ( i=0; i<in_symbols.length(); i++ )
    {
        index = in_symbols ( i );

        //feed in the appropriate symbols
        out_symbols ( 2*i ) = modulation_symbs_list ( 2*index );
        out_symbols ( 2*i+1 ) = modulation_symbs_list ( 2*index+1 );
    }
}

/*
**	void SPHERE_PACKING::modulate_symbols(const ivec &in_symbols, cvec &out_symbols)
**
**
**
**	Parameters:
**                 in_symbols     ==>     integer input symbols
**
**	Returns:   complex modulated symbols
**
**	Notes: use this when number of output symbols is half that of the input
**             i.e. each two input integer symbols are grouped to create one sp symbol
**                  the first of each pair is the least significant symbol
*/

void SPHERE_PACKING::modulate_symbols_diff_size ( const ivec &in_symbols, cvec &out_symbols )
{

    int i,j,nrof_blocks,index,m;

    out_symbols.set_size ( in_symbols.size(),false );

    m = round_i ( ( M_LOG2E* ( std::log ( ( double ) L ) ) ) );

    for ( i=0; i<in_symbols.length() /2; i++ )
    {
        //grouping two integer symbols to create one integer symbol
        //the first symbol is the least significant symbol
        index = in_symbols ( 2*i+1 ) *m + in_symbols ( 2*i );

        //feed in the appropriate symbols
        out_symbols ( 2*i ) = modulation_symbs_list ( 2*index );
        out_symbols ( 2*i+1 ) = modulation_symbs_list ( 2*index+1 );
    }

}



/*
**	void SPHERE_PACKING::modulate_symbols(const ivec &in_symbols, cmat &out_symbols)
**
**
**
**	Parameters:
**                 in_symbols     ==>     integer input symbols
**
**	Returns:   modulated symbols
**
**	Notes: N/A
*/

void SPHERE_PACKING::modulate_symbols ( const ivec &in_symbols, cmat &out_symbols )
{
    int i,j,nrof_blocks,index,nrof_symbs;

    out_symbols.set_size ( in_symbols.length(),nrof_symbs_per_block,false );

    for ( i=0; i<in_symbols.length(); i++ )
    {
        index = in_symbols ( i );
        //feed in the appropriate symbols
        out_symbols ( i,0 ) = modulation_symbs_list ( 2*index );
        out_symbols ( i,1 ) = modulation_symbs_list ( 2*index+1 );
    }
}



// demodulation of the received symbols Hard Decision
bvec SPHERE_PACKING::demodulate_bits ( const cvec &signal, vec &channel )
{
    int i,j,index;
    bvec rx_bits,temp_bits;
    cvec rx_signal = signal;
    vec temp;
    double temp_channel;

    // setting the bits vector to the right size as in page 46 log book 1
    rx_bits.set_size ( signal.length() /nrof_symbs_per_block*block_size,false );

    // setting the vector that will sample the input symbols block by block
    temp.set_size ( 2*nrof_symbs_per_block,false );

    // setting the binary temp_bits vector that will hold the binary bits block by block
    temp_bits.set_size ( block_size,false );



    //looping through the signal vector
    for ( i=0; i<signal.length() /nrof_symbs_per_block; i++ )
    {
        for ( j=0; j<nrof_symbs_per_block; j++ )
        {
            temp ( 2*j ) = rx_signal ( 2*i+j ).real();
            temp ( 2*j+1 ) = rx_signal ( 2*i+j ).imag();
        }


        // getting channel response -- assumming channel is constant for one space-time symbol
        temp_channel = channel ( 2*i );

        // now, we find the integer index with the smallest distance with temp
        index = find_smallest_distance ( temp,temp_channel );
        temp_bits = dec2bin ( block_size,index );

        //feeding in the demodulated bits to the right place in rx_bits
        for ( j=0; j<block_size; j++ )
        {
            rx_bits ( i*block_size+j ) = temp_bits ( block_size-1-j );
        }
    }
    return ( rx_bits );
}


// demodulation of the received symbols Hard Decision
void SPHERE_PACKING::demodulate_bits ( const cvec &signal, vec &channel, bvec &rx_bits )
{
    int i,j,index;
    bvec temp_bits;
    cvec rx_signal = signal;
    vec temp;
    double temp_channel;

    // setting the bits vector to the right size as in page 46 log book 1
    rx_bits.set_size ( signal.length() /nrof_symbs_per_block*block_size,false );

    // setting the vector that will sample the input symbols block by block
    temp.set_size ( 2*nrof_symbs_per_block,false );

    // setting the binary temp_bits vector that will hold the binary bits block by block
    temp_bits.set_size ( block_size,false );



    //looping through the signal vector
    for ( i=0; i<signal.length() /nrof_symbs_per_block; i++ )
    {
        for ( j=0; j<nrof_symbs_per_block; j++ )
        {
            temp ( 2*j ) = rx_signal ( 2*i+j ).real();
            temp ( 2*j+1 ) = rx_signal ( 2*i+j ).imag();
        }


        // getting channel response -- assumming channel is constant for one space-time symbol
        temp_channel = channel ( 2*i );

        // now, we find the integer index with the smallest distance with temp
        index = find_smallest_distance ( temp,temp_channel );
        temp_bits = dec2bin ( block_size,index );

        //feeding in the demodulated bits to the right place in rx_bits
        for ( j=0; j<block_size; j++ )
        {
            rx_bits ( i*block_size+j ) = temp_bits ( block_size-1-j );
        }
    }

}




// demodulation of the received integer symbols Hard Decision
bvec SPHERE_PACKING::demodulate_symbols ( ivec index )
{

    int i,j;
    bvec bits,temp_bits;

    // setting the bits vector to the right size
    bits.set_size ( index.length() *block_size,false );

    // setting the binary temp_bits vector that will hold the binary bits block by block
    temp_bits.set_size ( block_size,false );


    //looping through the index ivector
    for ( i=0; i<index.length(); i++ )
    {
        // now, we convert the integer index to binary bits
        temp_bits = dec2bin ( block_size,index ( i ) );

        //feeding in the demodulated bits to the right place in bits
        for ( j=0; j<block_size; j++ )
        {
            bits ( i*block_size+j ) = temp_bits ( block_size-1-j );
        }
    }
    return ( bits );


}


// convert integer symbols to binary bits
bvec SPHERE_PACKING::int_symbols_to_bin_bits ( ivec index,int blockSize )
{

    int i,j;
    bvec bits,temp_bits;

    // setting the bits vector to the right size
    bits.set_size ( index.length() *blockSize,false );

    // setting the binary temp_bits vector that will hold the binary bits block by block
    temp_bits.set_size ( blockSize,false );


    //looping through the index ivector
    for ( i=0; i<index.length(); i++ )
    {
        // now, we convert the integer index to binary bits
        temp_bits = dec2bin ( blockSize,index ( i ) );

        //feeding in the demodulated bits to the right place in bits
        for ( j=0; j<blockSize; j++ )
        {
            //bits(i*blockSize+j)= temp_bits(blockSize-1-j); //this is one way
            bits ( i*blockSize+j ) = temp_bits ( j );         //this is the opposite
        }
    }
    return ( bits );


}

// convert integer symbols to binary bits
void SPHERE_PACKING::int_symbols_to_bin_bits ( ivec index,int blockSize, bvec &bits )
{

    int i,j;
    bvec temp_bits;

    // setting the bits vector to the right size
    bits.set_size ( index.length() *blockSize,false );

    // setting the binary temp_bits vector that will hold the binary bits block by block
    temp_bits.set_size ( blockSize,false );


    //looping through the index ivector
    for ( i=0; i<index.length(); i++ )
    {
        // now, we convert the integer index to binary bits
        temp_bits = dec2bin ( blockSize,index ( i ) );

        //feeding in the demodulated bits to the right place in bits
        for ( j=0; j<blockSize; j++ )
        {
            //bits(i*blockSize+j)= temp_bits(blockSize-1-j); //this is one way
            bits ( i*blockSize+j ) = temp_bits ( j );         //this is the opposite
        }
    }

}


// convert binary bits to integer symbols
void SPHERE_PACKING::bin_bits_to_int_symbols ( bvec bits, int blockSize, ivec &i_symbols )
{
    bvec block;
    int i,j,nrof_blocks;

    nrof_blocks = floor_i ( bits.length() /blockSize );

    //check
    double test_size = ( ( double ) bits.length() ) / ( ( double ) blockSize );
    if ( nrof_blocks != test_size )
    {
        printf ( "\n Error in void SPHERE_PACKING::bin_bits_to_int_symbols() \n" );
        printf ( "\n bits.length() = %d must be multiple of blockSize = %d \n",bits.length(),blockSize );
        exit ( 1 );
    }

    i_symbols.set_size ( nrof_blocks,false );
    block.set_size ( blockSize,false );    // defining the binary vector that will take b bits a time to
    // convert them to an integer index




    for ( i=0; i<nrof_blocks; i++ )
    {
        //Group bits in a binary block of size "block_size"
        for ( j=0; j<blockSize; j++ )
        {
            block ( j ) = bits ( i*blockSize+j );
        }

        i_symbols ( i ) = bin2dec ( block,false );
    }

}

// convert a vector of LLR values to a matrix of two columns for Pr(x=0) and Pr(x=1) respectively
mat SPHERE_PACKING::llr_to_prob ( const vec &in_llr )
{
    int i;
    double L_value;
    mat tmp_prob;

    tmp_prob.set_size ( in_llr.size(),2,false );
    tmp_prob.clear();

    for ( i=0; i<in_llr.size(); i++ )
    {
        L_value = in_llr ( i );
        //first check for large LLR values
        if ( L_value > 100.0 ) L_value = 100.0;
        if ( L_value < -100.0 ) L_value = -100.0;

        //calculate Pr(x=0) or Pr(x=-1)
        tmp_prob ( i,0 ) = 1.0/ ( 1.0 + exp ( L_value ) );

        //cout << "L_value = " <<L_value << " ->  tmp_prob(i,0) = " << tmp_prob(i,0) << endl;

        //calculate Pr(x=1)
        tmp_prob ( i,1 ) = 1.0/ ( 1.0 + exp ( -L_value ) );
    }

    return ( tmp_prob );
}



//convert a two-column matrix representing Pr(x=0) and Pr(x=1) to a vector of LLR values
vec SPHERE_PACKING::prob_to_llr ( const mat &in_prob )
{
    int i;
    double P0,P1;

    vec tmp_llr;
    tmp_llr.set_size ( in_prob.rows(),false );


    for ( i=0; i<in_prob.rows(); i++ )
    {

        P1 = in_prob ( i,1 );
        P0 = in_prob ( i,0 );

        if ( P1 == 0 )
        {
            cout <<endl << "P0 = " << P0 << endl;
            cout <<endl << "P1 = " << P1 << endl;
            getchar();
        }

        if ( P0 == 0 )
        {
            cout <<endl << "P0 = " << P0 << endl;
            cout <<endl << "P1 = " << P1 << endl;
            getchar();
        }

        //if (P1 < 1e-4)  P1 = 1e-4;
        //if (P0 < 1e-4)  P0 = 1e-4;

        if ( P1 >= 0.9999999 )
        {

            tmp_llr ( i ) = +13.8;

        }
        else if ( P0  >= 0.9999999 )
        {

            tmp_llr ( i ) = -13.8;

        }
        else
        {

            //L(x) = log(x=1)/log(x=0)
            tmp_llr ( i ) = log ( P1/P0 );
        }
    }
    return ( tmp_llr );
}



// private
int SPHERE_PACKING::find_smallest_distance ( const vec &constellation, double channel_amplitude )
{
    double min_dist=1e100,distance;
    int i,j,index;
    vec legi_phasor;
    legi_phasor.set_size ( dimension,false );

    for ( i=0; i<L; i++ )
    {

        //get legitimate phasor
        legi_phasor = get_legi_phasor ( i );

        distance=0.0;
        for ( j=0; j<dimension; j++ )
        {
            distance += pow ( 1.0*channel_amplitude*legi_phasor ( j )-constellation ( j ),2 );
        }
        distance = sqrt ( distance );

        if ( distance < min_dist )
        {
            min_dist = distance;
            index = i;
        }
    }
    return ( index );
}


//  ******************  ORIGINAL  ********************************//

// demodulation of the received symbols Soft Decision
vec SPHERE_PACKING::demodulate_soft_bits ( const cvec &signal, vec &channel, double N0 )
{

    int l, i, j, k;
    double P0, P1, d0, d1, P0_max, P1_max;
    double d0min, d0min2, d1min, d1min2;
    double treshhold = -log ( DBL_EPSILON ); // To be sure that any precision is left in the calculatation
    double inv_N0 = 1.0/N0;

    vec soft_bits,temp,vec0,vec1;

    soft_bits.set_size ( block_size*signal.size() /nrof_symbs_per_block, false );
    temp.set_size ( dimension,false );
    vec0.set_size ( dimension,false );
    vec1.set_size ( dimension,false );

    for ( l=0; l<signal.size() /nrof_symbs_per_block; l++ )
    {
        for ( k=0; k<nrof_symbs_per_block; k++ )
        {
            temp ( 2*k )  = signal ( 2*l+k ).real();
            temp ( 2*k+1 ) = signal ( 2*l+k ).imag();
        }

        for ( i=0; i<block_size; i++ )
        {

            P0 = P1 = 0;
            P0_max = P1_max = -1e100;
            d0min = d0min2 = d1min = d1min2 = 1e100;
            for ( j=0; j< ( L/2 ); j++ )
            {
                vec0 = get_legi_phasor ( S0 ( i,j ) );
                vec1 = get_legi_phasor ( S1 ( i,j ) );

                d0 = d1 = 0;
                for ( k=0; k<dimension; k++ )
                {
                    d0 += pow ( ( temp ( k )-channel ( 2*l ) *vec0 ( k ) ),2.0 ) / ( channel ( 2*l ) *2*N0 ); // original
                    d1 += pow ( ( temp ( k )-channel ( 2*l ) *vec1 ( k ) ),2.0 ) / ( channel ( 2*l ) *2*N0 ); // original

                    //d0 += pow((temp(k)-vec0(k)),2.0); // modified
                    //d1 += pow((temp(k)-vec1(k)),2.0); // modified
                }

                if ( d0 < d0min )
                {
                    d0min2 = d0min;
                    d0min = d0;
                }
                if ( d1 < d1min )
                {
                    d1min2 = d1min;
                    d1min = d1;
                }

                //P0 += exp(-d0); // original
                //P1 += exp(-d1); // original

                P0 = -d0 ; // max-log
                P1 = -d1 ; // max-log

                if ( P0 > P0_max )
                {
                    P0_max = P0;    // max-log
                }
                if ( P1 > P1_max )
                {
                    P1_max = P1;    // max-log
                }
            }

            // if ( (d0min2-d0min) > treshhold && (d1min2-d1min) > treshhold ) //
            //{  //
            //soft_bits(l*block_size+i) = -d0min + d1min;
            //} else{  //
            //soft_bits(l*block_size+i) = log(P0/P1); // original
            soft_bits ( l*block_size+i ) =  P0_max - P1_max; // max-log

            //}  //
        }
    }
    return ( soft_bits );
}





//  ****************** soft demodulation for STBC ********************************//
//
// For STBC systems with G2 with apriori information and channel envelope
//
//
//
vec SPHERE_PACKING::demodulate_soft_bits ( const cvec &signal, vec &channel, double N0, vec &L_apriori,int metric )
{


    int l, i, j, k,m;
    double P0, P1, d0, d1, P0_max, P1_max;
    double d0min, d0min2, d1min, d1min2;
    double treshhold = -log ( DBL_EPSILON ); // To be sure that any precision is left in the calculatation
    double inv_N0 = 1.0/N0;
    double sum_L_a_0, sum_L_a_1;

    vec soft_bits,temp,vec0,vec1;
    bvec bits_0,bits_1, temp_bits_0, temp_bits_1;

    soft_bits.set_size ( block_size*signal.size() /nrof_symbs_per_block, false );
    temp.set_size ( dimension,false );
    vec0.set_size ( dimension,false );
    vec1.set_size ( dimension,false );

    // setting the binary temp_bits vectors that will hold the binary bits block by block
    temp_bits_0.set_size ( block_size,false );
    temp_bits_1.set_size ( block_size,false );
    bits_0.set_size ( block_size,false );
    bits_1.set_size ( block_size,false );


    for ( l=0; l<signal.size() /nrof_symbs_per_block; l++ )
    {
        for ( k=0; k<nrof_symbs_per_block; k++ )
        {
            temp ( 2*k )  = signal ( 2*l+k ).real();
            temp ( 2*k+1 ) = signal ( 2*l+k ).imag();
        }

        for ( i=0; i<block_size; i++ )
        {
            P0 = P1 = 0;
            P0_max = P1_max = -1e100;

            d0min = d0min2 = d1min = d1min2 = 1e100;

            for ( j=0; j< ( L/2 ); j++ )
            {

                // calculating the required La(x_j)'s

                // first get the binary digits for each legitimate symbol
                temp_bits_0 = dec2bin ( block_size,S0 ( i,j ) );
                temp_bits_1 = dec2bin ( block_size,S1 ( i,j ) );

                //Arranging the binary bits to the correct order
                for ( m=0; m<block_size; m++ )
                {
                    bits_0 ( m ) = temp_bits_0 ( block_size-1-m );
                    bits_1 ( m ) = temp_bits_1 ( block_size-1-m );
                }

                sum_L_a_0 = 0.0;
                sum_L_a_1 = 0.0;

                for ( m=0; m<block_size; m++ )
                {
                    if ( m != i )
                    {
                        if ( bits_0 ( m ) == 1 )
                        {
                            sum_L_a_0 += L_apriori ( l*block_size+m );
                        }
                        if ( bits_1 ( m ) == 1 )
                        {
                            sum_L_a_1 += L_apriori ( l*block_size+m );
                        }
                    }
                }

                vec0 = get_legi_phasor ( S0 ( i,j ) );
                vec1 = get_legi_phasor ( S1 ( i,j ) );

                d0 = d1 = 0;
                for ( k=0; k<dimension; k++ )
                {
                    d0 += pow ( ( temp ( k )-channel ( 2*l ) *vec0 ( k ) ),2.0 ) / ( channel ( 2*l ) *2*N0 ); //
                    d1 += pow ( ( temp ( k )-channel ( 2*l ) *vec1 ( k ) ),2.0 ) / ( channel ( 2*l ) *2*N0 ); //
                }

                if ( d0 < d0min )
                {
                    d0min2 = d0min;
                    d0min = d0;
                }
                if ( d1 < d1min )
                {
                    d1min2 = d1min;
                    d1min = d1;
                }

                if ( metric == EXACT )
                {

                    P0 += exp ( -d0 + sum_L_a_0 ); // EXACT
                    P1 += exp ( -d1 + sum_L_a_1 ); // EXACT

                }
                else if ( metric == MAXLOG )
                {

                    P0 = -d0 + sum_L_a_0; // max-log
                    P1 = -d1 + sum_L_a_1; // max-log

                    if ( P0 > P0_max )
                    {
                        P0_max = P0;    // max-log
                    }
                    if ( P1 > P1_max )
                    {
                        P1_max = P1;    // max-log
                    }

                }
                else
                {

                    cout << endl << "ERROR: SPHERE_PACKING::demodulate_soft_bits():" << endl;
                    cout << "undefined metric -- expect 0 or 1 only" << endl;
                    exit ( 0 );
                }
            }

            if ( metric == EXACT )
            {

                soft_bits ( l*block_size+i ) = L_apriori ( l*block_size+i ) + log ( P1/P0 ); //EXACT


                //fixing large values
                //if (soft_bits(l*block_size+i) >  1000) soft_bits(l*block_size+i)= +1000.0;
                //if (soft_bits(l*block_size+i) < -1000) soft_bits(l*block_size+i)= -1000.0;


            }
            else if ( metric == MAXLOG )
            {

                soft_bits ( l*block_size+i ) = L_apriori ( l*block_size+i ) - P0_max + P1_max; // max-log

            }
            else
            {

                cout << endl << "ERROR: SPHERE_PACKING::demodulate_soft_bits():" << endl;
                cout << "undefined metric -- expect 0 or 1 only" << endl;
                exit ( 0 );;
            }
        }
    }
    return ( soft_bits );
}



//  ****************** soft demodulation for STBC ********************************//
//
// For STBC systems with G2 with apriori information and channel envelope
//
//
//
void SPHERE_PACKING::demodulate_soft_bits ( const cvec &signal, vec &channel, double N0, vec &L_aposteriori, vec &L_apriori,int metric )
{


    int l, i, j, k,m;
    double P0, P1, d0, d1, P0_log, P1_log, P0_log_approx, P1_log_approx, P0_max, P1_max;
    double d0min, d0min2, d1min, d1min2;
    double treshhold = -log ( DBL_EPSILON ); // To be sure that any precision is left in the calculatation
    double inv_N0 = 1.0/N0;
    double sum_L_a_0, sum_L_a_1;

    vec temp,vec0,vec1;
    bvec bits_0,bits_1, temp_bits_0, temp_bits_1;

    L_aposteriori.set_size ( block_size*signal.size() /nrof_symbs_per_block, false );
    temp.set_size ( dimension,false );
    vec0.set_size ( dimension,false );
    vec1.set_size ( dimension,false );

    // setting the binary temp_bits vectors that will hold the binary bits block by block
    temp_bits_0.set_size ( block_size,false );
    temp_bits_1.set_size ( block_size,false );
    bits_0.set_size ( block_size,false );
    bits_1.set_size ( block_size,false );


    for ( l=0; l<signal.size() /nrof_symbs_per_block; l++ )
    {
        for ( k=0; k<nrof_symbs_per_block; k++ )
        {
            temp ( 2*k )  = signal ( 2*l+k ).real();
            temp ( 2*k+1 ) = signal ( 2*l+k ).imag();
        }

        for ( i=0; i<block_size; i++ )
        {
            P0 = P1 = 0;
            P0_log = P1_log = P0_log_approx = P1_log_approx = -100000;
            P0_max = P1_max = -1e100;

            d0min = d0min2 = d1min = d1min2 = 1e100;

            for ( j=0; j< ( L/2 ); j++ )
            {

                // calculating the required La(x_j)'s

                // first get the binary digits for each legitimate symbol
                temp_bits_0 = dec2bin ( block_size,S0 ( i,j ) );
                temp_bits_1 = dec2bin ( block_size,S1 ( i,j ) );

                //Arranging the binary bits to the correct order
                for ( m=0; m<block_size; m++ )
                {
                    bits_0 ( m ) = temp_bits_0 ( block_size-1-m );
                    bits_1 ( m ) = temp_bits_1 ( block_size-1-m );
                }

                sum_L_a_0 = 0.0;
                sum_L_a_1 = 0.0;

                for ( m=0; m<block_size; m++ )
                {
                    if ( m != i )
                    {
                        if ( bits_0 ( m ) == 1 )
                        {
                            sum_L_a_0 += L_apriori ( l*block_size+m );
                        }
                        if ( bits_1 ( m ) == 1 )
                        {
                            sum_L_a_1 += L_apriori ( l*block_size+m );
                        }
                    }
                }

                vec0 = get_legi_phasor ( S0 ( i,j ) );
                vec1 = get_legi_phasor ( S1 ( i,j ) );

                d0 = d1 = 0;
                for ( k=0; k<dimension; k++ )
                {
                    d0 += pow ( ( temp ( k )-channel ( 2*l ) *vec0 ( k ) ),2.0 ) / ( channel ( 2*l ) *2*N0 );
                    d1 += pow ( ( temp ( k )-channel ( 2*l ) *vec1 ( k ) ),2.0 ) / ( channel ( 2*l ) *2*N0 );

                }

                if ( d0 < d0min )
                {
                    d0min2 = d0min;
                    d0min = d0;
                }
                if ( d1 < d1min )
                {
                    d1min2 = d1min;
                    d1min = d1;
                }

                if ( metric == EXACT )
                {

                    P0 += exp ( -d0 + sum_L_a_0 ); // EXACT
                    P1 += exp ( -d1 + sum_L_a_1 ); // EXACT

                }
                else if ( metric == LOG )
                {

                    P0_log = jacolog ( P0_log, ( -d0 + sum_L_a_0 ) ); // LOG
                    P1_log = jacolog ( P1_log, ( -d1 + sum_L_a_1 ) ); // LOG

                }
                else if ( metric == LOG_APPROX )
                {

                    P0_log_approx = jacolog_approx ( P0_log_approx, ( -d0 + sum_L_a_0 ) ); // LOG Approx
                    P1_log_approx = jacolog_approx ( P1_log_approx, ( -d1 + sum_L_a_1 ) ); // LOG Approx

                }
                else if ( metric == MAXLOG )
                {

                    P0 = -d0 + sum_L_a_0; // max-log
                    P1 = -d1 + sum_L_a_1; // max-log

                    if ( P0 > P0_max )
                    {
                        P0_max = P0;    // max-log
                    }
                    if ( P1 > P1_max )
                    {
                        P1_max = P1;    // max-log
                    }

                }
                else
                {

                    cout << endl << "ERROR: SPHERE_PACKING::demodulate_soft_bits():" << endl;
                    cout << "undefined metric -- expect 0 or 1 only" << endl;
                    exit ( 0 );;
                }
            }

            if ( metric == EXACT )
            {

                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) + log ( P1/P0 ); //EXACT

            }
            else if ( metric == LOG )
            {

                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) + P1_log - P0_log; // LOG

            }
            else if ( metric == LOG_APPROX )
            {

                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) + P1_log_approx - P0_log_approx; // LOG Approx

            }
            else if ( metric == MAXLOG )
            {

                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) + P1_max - P0_max; // max-log

            }
            else
            {

                cout << endl << "ERROR: SPHERE_PACKING::demodulate_soft_bits():" << endl;
                cout << "undefined metric -- expect 0 or 1 or 2 or 3 only" << endl;
                exit ( 0 );
            }
        }
    }
}


//  ****************** soft demodulation for STBC ********************************//
//
// For STBC systems with G2 with apriori information and without channel envelope
//
//
//
void SPHERE_PACKING::demodulate_soft_bits ( const cvec &signal,  double N0, vec &L_aposteriori, vec &L_apriori,int metric )
{



    int l, i, j, k,m;
    double P0, P1, d0, d1, P0_max, P1_max;
    double d0min, d0min2, d1min, d1min2;
    double treshhold = -log ( DBL_EPSILON ); // To be sure that any precision is left in the calculatation
    double inv_N0 = 1.0/N0;
    double sum_L_a_0, sum_L_a_1;

    vec temp,vec0,vec1;
    bvec bits_0,bits_1, temp_bits_0, temp_bits_1;

    L_aposteriori.set_size ( block_size*signal.size() /nrof_symbs_per_block, false );
    temp.set_size ( dimension,false );
    vec0.set_size ( dimension,false );
    vec1.set_size ( dimension,false );

    // setting the binary temp_bits vectors that will hold the binary bits block by block
    temp_bits_0.set_size ( block_size,false );
    temp_bits_1.set_size ( block_size,false );
    bits_0.set_size ( block_size,false );
    bits_1.set_size ( block_size,false );


    for ( l=0; l<signal.size() /nrof_symbs_per_block; l++ )
    {
        for ( k=0; k<nrof_symbs_per_block; k++ )
        {
            temp ( 2*k )  = signal ( 2*l+k ).real();
            temp ( 2*k+1 ) = signal ( 2*l+k ).imag();
        }

        for ( i=0; i<block_size; i++ )
        {
            P0 = P1 = 0;
            P0_max = P1_max = -1e100;

            d0min = d0min2 = d1min = d1min2 = 1e100;

            for ( j=0; j< ( L/2 ); j++ )
            {

                // calculating the required La(x_j)'s

                // first get the binary digits for each legitimate symbol
                temp_bits_0 = dec2bin ( block_size,S0 ( i,j ) );
                temp_bits_1 = dec2bin ( block_size,S1 ( i,j ) );

                //Arranging the binary bits to the correct order
                for ( m=0; m<block_size; m++ )
                {
                    bits_0 ( m ) = temp_bits_0 ( block_size-1-m );
                    bits_1 ( m ) = temp_bits_1 ( block_size-1-m );
                }

                sum_L_a_0 = 0.0;
                sum_L_a_1 = 0.0;

                for ( m=0; m<block_size; m++ )
                {
                    if ( m != i )
                    {
                        if ( bits_0 ( m ) == 1 )
                        {
                            sum_L_a_0 += L_apriori ( l*block_size+m );
                        }
                        if ( bits_1 ( m ) == 1 )
                        {
                            sum_L_a_1 += L_apriori ( l*block_size+m );
                        }
                    }
                }

                vec0 = get_legi_phasor ( S0 ( i,j ) );
                vec1 = get_legi_phasor ( S1 ( i,j ) );

                d0 = d1 = 0;
                for ( k=0; k<dimension; k++ )
                {
                    d0 += pow ( ( temp ( k )-vec0 ( k ) ),2.0 ) / ( 2*N0 ); //
                    d1 += pow ( ( temp ( k )-vec1 ( k ) ),2.0 ) / ( 2*N0 ); //
                }

                if ( d0 < d0min )
                {
                    d0min2 = d0min;
                    d0min = d0;
                }
                if ( d1 < d1min )
                {
                    d1min2 = d1min;
                    d1min = d1;
                }

                if ( metric == EXACT )
                {

                    P0 += exp ( -d0 + sum_L_a_0 ); // EXACT
                    P1 += exp ( -d1 + sum_L_a_1 ); // EXACT

                }
                else if ( metric == MAXLOG )
                {

                    P0 = -d0 + sum_L_a_0; // max-log
                    P1 = -d1 + sum_L_a_1; // max-log

                    if ( P0 > P0_max )
                    {
                        P0_max = P0;    // max-log
                    }
                    if ( P1 > P1_max )
                    {
                        P1_max = P1;    // max-log
                    }

                }
                else
                {

                    cout << endl << "ERROR: SPHERE_PACKING::demodulate_soft_bits():" << endl;
                    cout << "undefined metric -- expect 0 or 1 only" << endl;
                    exit ( 0 );;
                }
            }

            if ( metric == EXACT )
            {

                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) + log ( P1/P0 ); //EXACT


                //fixing large values
                //if (L_aposteriori(l*block_size+i) >  1000) L_aposteriori(l*block_size+i)= +1000.0;
                //if (L_aposteriori(l*block_size+i) < -1000) L_aposteriori(l*block_size+i)= -1000.0;


            }
            else if ( metric == MAXLOG )
            {

                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) - P0_max + P1_max; // max-log

            }
            else
            {

                cout << endl << "ERROR: SPHERE_PACKING::demodulate_soft_bits():" << endl;
                cout << "undefined metric -- expect 0 or 1 only" << endl;
                exit ( 0 );;
            }
        }
    }
}






//  *************************** soft demodulation  ***********************************//
//
//   For STBC with G2, Symbol-based demapping with a priori info and channel info
//
//
//
void SPHERE_PACKING::demodulate_soft_symbols ( const cvec &signal, mat &soft_output, vec &channel, double N0, mat &apriori, int iter_no )
{
    int l, i, j, k,m;
    double d;
    double treshhold = -log ( DBL_EPSILON ) * N0; // To be sure that any precision is left in the calculatation
    double inv_N0 = 1.0/N0;
    double total_prob;
    double dmin, dmin2;


    soft_output.set_size ( signal.size() /nrof_symbs_per_block,L,false );

    vec rx_symb, legi_symb;
    rx_symb.set_size ( dimension,false );
    legi_symb.set_size ( dimension,false );


    //loop through received symbols
    for ( l=0; l<signal.size() /nrof_symbs_per_block; l++ )
    {

        if ( iter_no == 0 )
        {
            prob_vec.set_size ( signal.size() /nrof_symbs_per_block,L,false );

            dmin = dmin2 = 1e100;

            //convert each two symbols to a 4-dimensional sphere-packing symbol
            for ( k=0; k<nrof_symbs_per_block; k++ )
            {
                rx_symb ( 2*k )  = signal ( 2*l+k ).real();
                rx_symb ( 2*k+1 ) = signal ( 2*l+k ).imag();
            }

            //loop through all Legitimate symbols list
            total_prob = 0.0;
            for ( j=0; j<L; j++ )
            {
                legi_symb = get_legi_phasor ( j );

                //calculate d, which is the exponent term of the conditional pdf
                d = 0.0;
                for ( k=0; k<dimension; k++ )

                {
                    d += pow ( ( rx_symb ( k )-channel ( 2*l ) *legi_symb ( k ) ),2.0 ) / ( channel ( 2*l ) *2*N0 );
                }

                if ( d < dmin )
                {
                    dmin2 = dmin;
                    dmin = d;
                }
                prob_vec ( l,j ) = exp ( -d );
                if ( prob_vec ( l,j ) < 0.0000001 ) prob_vec ( l,j ) = 0.0000001;

            } //loop back to calculate probability of another legitimate symbol

        }

        //add apriori knowledge of current received sphere packing symbol
        total_prob = 0.0;
        for ( j=0; j<L; j++ )
        {
            prob_vec ( l,j ) *= apriori ( l,j );
            total_prob += prob_vec ( l,j );
        }

        //normalise the probabilites of the current received sphere packing symbol
        //such that they add up to unity and FILL in its soft_output
        for ( j=0; j<L; j++ )
        {
            prob_vec ( l,j ) *= ( 1.0/total_prob );
            soft_output ( l,j ) = prob_vec ( l,j );
        }

    } //loop back to calculate probabilities of another received sphere packing symbol

    iterations_counter++;
}


//  *************************** soft demodulation  ***********************************//
//
//   For STBC with G2, Symbol-based demapping with a priori info and "WITH" channel info
//   when different constellation size is used for sphere packing from previous block
//
//   created for my applications
//
//
void SPHERE_PACKING::demodulate_soft_symbols_diff_size ( const cvec &signal, mat &soft_output, vec &channel, double N0, mat &apriori )
{
    int l, i, j, k,m;
    double d;
    double inv_N0 = 1.0/N0;
    double total_prob,total_prob_1st,total_prob_2nd;

    m = round_i ( log2 ( L ) );

    soft_output.set_size ( signal.size(),m,false );
    soft_output.clear();

    vec rx_symb, legi_symb;
    rx_symb.set_size ( dimension,false );
    legi_symb.set_size ( dimension,false );

    vec prob_vec,prob_vec_1st,prob_vec_2nd;
    prob_vec.set_size ( L,false );
    prob_vec_1st.set_size ( m,false );
    prob_vec_2nd.set_size ( m,false );

    //loop through received symbols
    for ( l=0; l<signal.size() /nrof_symbs_per_block; l++ )
    {

        //convert each two symbols to a 4-dimensional sphere-packing symbol
        for ( k=0; k<nrof_symbs_per_block; k++ )
        {
            rx_symb ( 2*k )  = signal ( 2*l+k ).real();
            rx_symb ( 2*k+1 ) = signal ( 2*l+k ).imag();
        }

        //loop through all Legitimate symbols list
        total_prob = 0.0;
        for ( j=0; j<L; j++ )
        {
            legi_symb = get_legi_phasor ( j );

            //calculate d, which is the exponent term of the conditional pdf
            d = 0.0;
            for ( k=0; k<dimension; k++ )

            {
                d += pow ( ( rx_symb ( k )-channel ( 2*l ) *legi_symb ( k ) ),2.0 ) / ( channel ( 2*l ) *2*N0 );
            }

            prob_vec ( j ) = exp ( -d );
            if ( prob_vec ( j ) < 0.0000001 ) prob_vec ( j ) = 0.0000001;
            total_prob += prob_vec ( j );

        } //loop back to calculate probability of another legitimate symbol

        //normalise the probabilites of the received sphere packing symbol such that they add up to unity
        prob_vec = ( 1.0/total_prob ) * prob_vec;

        //Convert the probabilities from 'L' to 'log2(L)' and add the apriori info
        prob_vec_1st.clear();
        prob_vec_2nd.clear();
        total_prob_1st = 0.0;
        total_prob_2nd = 0.0;

        for ( j=0; j<m; j++ )
        {
            for ( k=0; k<m; k++ )
            {
                prob_vec_1st ( j ) += prob_vec ( m*k+j ) * apriori ( 2*l,j );
                prob_vec_2nd ( j ) += prob_vec ( m*j+k ) * apriori ( 2*l+1,j );

            }
            total_prob_1st += prob_vec_1st ( j );
            total_prob_2nd += prob_vec_2nd ( j );
        }

        //normalise the probabilites such that they add up to unity
        prob_vec_1st = ( 1.0/total_prob_1st ) * prob_vec_1st;
        prob_vec_2nd = ( 1.0/total_prob_2nd ) * prob_vec_2nd;

        //Fill in the soft_output for the current sphere packing symbol
        for ( j=0; j<m; j++ )
        {
            soft_output ( 2*l,j ) = prob_vec_1st ( j );
            soft_output ( 2*l+1,j ) = prob_vec_2nd ( j );
        }

    } //loop back to calculate probabilities of another received sphere packing symbol

    iterations_counter++;

}





//  *************************** soft demodulation  ***********************************//
//
//   For STBC with G2, Symbol-based demapping with a priori info "WITHOUT" channel info
//   when different constellation size is used for sphere packing from previous block
//
//   created for Ming's SDMA
//
//
void SPHERE_PACKING::demodulate_soft_symbols_diff_size ( const cvec &signal, mat &soft_output, double N0, mat &apriori, int iter_no )
{

    int l, i, j, k,m;
    double d;
    double inv_N0 = 1.0/N0;
    double total_prob,total_prob_1st,total_prob_2nd, max;

    m = round_i ( ( M_LOG2E* ( std::log ( ( double ) L ) ) ) );

    soft_output.set_size ( signal.size(),m,false );
    soft_output.clear();

    vec rx_symb, legi_symb;
    rx_symb.set_size ( dimension,false );
    legi_symb.set_size ( dimension,false );

    vec prob_vec_1st,prob_vec_2nd;
    prob_vec_1st.set_size ( m,false );
    prob_vec_2nd.set_size ( m,false );


    //loop through received symbols
    for ( l=0; l<signal.size() /nrof_symbs_per_block; l++ )
    {

        //calculate unchanging metrics only during first iteration
        if ( iter_no == 0 )
        {
            prob_vec.set_size ( signal.size() /nrof_symbs_per_block,L,false );

            //convert each two symbols to a 4-dimensional sphere-packing symbol
            for ( k=0; k<nrof_symbs_per_block; k++ )
            {
                rx_symb ( 2*k )  = signal ( 2*l+k ).real();
                rx_symb ( 2*k+1 ) = signal ( 2*l+k ).imag();
            }

            //loop through all Legitimate symbols list
            total_prob = 0.0;
            // mike
            max=-100000;
            for ( j=0; j<L; j++ )
            {
                legi_symb = get_legi_phasor ( j );

                //calculate d, which is the exponent term of the conditional pdf
                d = 0.0;
                for ( k=0; k<dimension; k++ )
                {
                    //22/02/2005 mj02r:
                    //d += pow((rx_symb(k)-legi_symb(k)),2.0)/(2*N0);
                    d += pow ( ( rx_symb ( k )-legi_symb ( k ) ),2.0 ) /N0;
                }

                ////mike
                prob_vec ( l,j ) = -d;
                if ( prob_vec ( l,j ) >max ) max=prob_vec ( l,j );

                /*
                prob_vec(l,j) = exp(-d);
                if (prob_vec(l,j) < 0.0000001) prob_vec(l,j) = 0.0000001;
                total_prob += prob_vec(l,j);
                     */

            } //loop back to calculate probability of another legitimate symbol

            //mike
            for ( j=0; j<L; j++ ) prob_vec ( l,j ) -= max;

            //normalise the probabilites of the received sphere packing symbol such that they add up to unity
            //prob_vec = (1.0/total_prob) * prob_vec;

        }


        // end here //

        //Convert the probabilities from 'L' to 'log2(L)' and add the apriori info
        prob_vec_1st.clear();
        prob_vec_2nd.clear();
        //total_prob_1st = 0.0;
        //total_prob_2nd = 0.0;
        //mike
        total_prob_1st=total_prob_2nd=-100000;

        for ( j=0; j<m; j++ )
        {
            prob_vec_1st ( j ) =prob_vec_2nd ( j ) =-100000;

            for ( k=0; k<m; k++ )
            {

                /*
                prob_vec_1st(j) += prob_vec(l,m*k+j)* apriori(2*l,j);
                prob_vec_2nd(j) += prob_vec(l,m*j+k)* apriori(2*l+1,j);
                     */

                //mike
                prob_vec_1st ( j ) = jacolog_approx ( prob_vec_1st ( j ), prob_vec ( l,m*k+j ) + apriori ( 2*l,j ) );
                prob_vec_2nd ( j ) = jacolog_approx ( prob_vec_2nd ( j ), prob_vec ( l,m*j+k ) + apriori ( 2*l+1,j ) );
            }

            //mike
            //total_prob_1st += prob_vec_1st(j);
            //total_prob_2nd += prob_vec_2nd(j);

            if ( total_prob_1st < prob_vec_1st ( j ) )
            {
                total_prob_1st = prob_vec_1st ( j );
            }
            if ( total_prob_2nd < prob_vec_2nd ( j ) )
            {
                total_prob_2nd = prob_vec_2nd ( j );
            }
        }

        //normalise the probabilites such that they add up to unity
        //prob_vec_1st = (1.0/total_prob_1st) * prob_vec_1st;
        //prob_vec_2nd = (1.0/total_prob_2nd) * prob_vec_2nd;
        for ( j=0; j<m; j++ )
        {
            prob_vec_1st ( j ) -= total_prob_1st;
            prob_vec_2nd ( j ) -= total_prob_2nd;
        }

        //Fill in the soft_output for the current sphere packing symbol
        for ( j=0; j<m; j++ )
        {
            soft_output ( 2*l,j ) = prob_vec_1st ( j );
            soft_output ( 2*l+1,j ) = prob_vec_2nd ( j );
        }


        //////////////mj02r
        /*
        cout<<"prob_vec_1st = "<<prob_vec_1st<<endl;
        getchar();
        cout<<"prob_vec_2nd = "<<prob_vec_2nd<<endl;
        getchar();
             */

    } //loop back to calculate probabilities of another received sphere packing symbol

    iterations_counter++;


}



double SPHERE_PACKING::jacolog ( double x, double y )
{
    double r;

    if ( x>y ) r = x + log ( 1 + exp ( y - x ) );
    else    r = y + log ( 1 + exp ( x - y ) );

    return r;
}


double SPHERE_PACKING::jacolog_approx ( double x, double y )
{
    double r;
    double diff;

    if ( x>y )
    {
        r = x;
        diff=x-y;
    }
    else
    {
        r = y;
        diff=y-x;
    }

    if ( diff > 3.7 )      r += 0.00;
    else if ( diff > 2.25 ) r += 0.05;
    else if ( diff > 1.5 ) r += 0.15;
    else if ( diff > 1.05 ) r += 0.25;
    else if ( diff > 0.7 ) r += 0.35;
    else if ( diff > 0.43 ) r += 0.45;
    else if ( diff > 0.2 ) r += 0.55;
    else                 r += 0.65;

    return r;
}


//  ****************** soft demodulation for STTC ********************************//
//
// For STTC systems with 2Tx and 1Rx
// See page 41 and 45 of log book
// It returns soft probabilities for each symbol
//
//  Symbol-by-Symbol
//
mat SPHERE_PACKING::demodulate_soft_symbols ( const cvec &signal, cmat *channel, double N0 )
{


    int i, j, k,m;
    double inv_N0 = 1.0/N0;
    complex<double> c1,c2,s;
    cmat H;
    cvec h;
    mat soft_prob;

    h.set_size ( nrof_Tx, false );
    soft_prob.set_size ( signal.size(),L, false );

    for ( m=0; m<signal.size(); m++ )
    {
        //Getting the current channel response matrix
        H = channel[m];

        //Grouping the channel response according to the general equation in page 46 of log book
        for ( i=0; i<nrof_Tx; i++ )
        {
            for ( j=0; j<nrof_Rx; j++ )
            {
                h ( i ) += H ( i,j );
            }
        }

        for ( i=0; i<L; i++ )
        {
            //looping through all L legitimate sphere packing symbols
            c1 = modulation_symbs_list ( 2*i );
            c2 = modulation_symbs_list ( 2*i+1 );

            //modifing the legitimate symbol by the channel response ** assuming 2Tx **
            //
            s = h ( 0 ) *c1+h ( 1 ) *c2;

            soft_prob ( m,i ) = exp ( -pow ( abs ( signal ( m )-s ),2 ) *inv_N0 );
        }
    }
    return ( soft_prob );

}







// get the legitimate phasor coordinates of the given index
vec SPHERE_PACKING::get_legi_phasor ( int index )
{

    int k;
    vec coordinates;
    coordinates.set_size ( dimension,false );


    for ( k=0; k<dimension; k++ )
    {
        coordinates ( k ) = ( double ) norm_factor*sphere_packing_matrix ( index,k );
    }
    return ( coordinates );
}




//
//
// input   : any arbitrary symbols cvec of length nrof_symbs_per_block
// returns : the most probable legitimate symbols
//
//
//

cvec SPHERE_PACKING::get_Legi_symbs_set ( const cvec &rx_symbs_set )
{
    int j,index;
    vec temp;
    cvec temp_legi_symbs;

    temp.set_size ( 2*nrof_symbs_per_block,false );
    temp_legi_symbs.set_size ( nrof_symbs_per_block,false );

    for ( j=0; j<nrof_symbs_per_block; j++ )
    {
        temp ( 2*j ) = rx_symbs_set ( j ).real();
        temp ( 2*j+1 ) = rx_symbs_set ( j ).imag();
    }


    // now, we find the integer index with the smallest distance with temp from the sphere packing matrix
    //index = find_smallest_distance(temp);


    // from the index, we create the complex symbols
    for ( j=0; j<nrof_symbs_per_block; j++ )
    {
        temp_legi_symbs ( j ) = double_complex ( sphere_packing_matrix ( index,2*j ),sphere_packing_matrix ( index,2*j+1 ) );

    }
    //cout <<"rx   "<< rx_symbs_set<<endl;
    //cout <<"Leg  "<< temp_legi_symbs*norm_factor<<endl;
    //cout <<"Leg+ "<< temp_legi_symbs<<endl;


    return ( temp_legi_symbs );
}




/*--------------------------------------------------------------------
	------------------- Class: SPHERE_PACKING -------------------
	--------------------------------------------------------------------*/

/*
** my_QPSK::my_QPSK(void)
**
*/
my_QPSK::my_QPSK ( int in_qpsk_mapping )
{
    int i,index=0;
    int k,count0,count1;
    bvec bits ( 2 );

    nrof_legi_symbols = 4;
    block_size = 2;          //number of bits in a symbol

    qpsk_matrix.set_size ( nrof_legi_symbols,2,false );

    qpsk_mapping = in_qpsk_mapping;

    double ONE_DIV_SQRT2= 0.70710678118654752440;


    switch ( qpsk_mapping )
    {
    case 0:
        //gray mapping

        qpsk_matrix ( 0,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 0,1 ) = ONE_DIV_SQRT2;

        qpsk_matrix ( 1,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 1,1 ) = -1*ONE_DIV_SQRT2;

        qpsk_matrix ( 2,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 2,1 ) = ONE_DIV_SQRT2;

        qpsk_matrix ( 3,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 3,1 ) = -1*ONE_DIV_SQRT2;
        break;

    case 1:
        //set partitioning 1
        qpsk_matrix ( 0,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 0,1 ) = ONE_DIV_SQRT2;

        qpsk_matrix ( 1,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 1,1 ) = -1*ONE_DIV_SQRT2;

        qpsk_matrix ( 2,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 2,1 ) = -1*ONE_DIV_SQRT2;

        qpsk_matrix ( 3,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 3,1 ) = ONE_DIV_SQRT2;
        break;

    case 2:
        //set partitioning 2
        qpsk_matrix ( 0,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 0,1 ) = -1*ONE_DIV_SQRT2;

        qpsk_matrix ( 1,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 1,1 ) = ONE_DIV_SQRT2;

        qpsk_matrix ( 2,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 2,1 ) = ONE_DIV_SQRT2;

        qpsk_matrix ( 3,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 3,1 ) = -1*ONE_DIV_SQRT2;
        break;

    case 3:
        //set partitioning 3
        qpsk_matrix ( 0,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 0,1 ) = ONE_DIV_SQRT2;

        qpsk_matrix ( 1,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 1,1 ) = ONE_DIV_SQRT2;

        qpsk_matrix ( 2,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 2,1 ) = -1*ONE_DIV_SQRT2;

        qpsk_matrix ( 3,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 3,1 ) = -1*ONE_DIV_SQRT2;
        break;

    case 4:
        //set partitioning 4
        qpsk_matrix ( 0,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 0,1 ) = -1*ONE_DIV_SQRT2;

        qpsk_matrix ( 1,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 1,1 ) = -1*ONE_DIV_SQRT2;

        qpsk_matrix ( 2,0 ) = -1*ONE_DIV_SQRT2;
        qpsk_matrix ( 2,1 ) = ONE_DIV_SQRT2;

        qpsk_matrix ( 3,0 ) = ONE_DIV_SQRT2;
        qpsk_matrix ( 3,1 ) = ONE_DIV_SQRT2;
        break;

    default:
        cout << endl << "ERROR in my_QPSK(): mapping = " << qpsk_mapping <<  "  is out of range " << endl;
        exit ( 0 );
    }

    // Calculate the bit pattern "bitmap"
    bitmap.set_size ( nrof_legi_symbols,block_size,false );


    for ( i=0; i<nrof_legi_symbols; i++ )
    {
        bitmap.set_row ( i,dec2bin ( block_size,i ) );
    }

    // Calculate the soft bit mapping matrices S0 and S1
    S0.set_size ( block_size,nrof_legi_symbols/2,false );
    S1.set_size ( block_size,nrof_legi_symbols/2,false );

    for ( k=0; k<block_size; k++ )
    {
        count0 = 0;
        count1 = 0;
        for ( index=0; index<nrof_legi_symbols; index++ )
        {
            bits = bitmap.get_row ( index );

            if ( bits ( ( block_size-1 )-k ) ==bin ( 0 ) )
            {
                S0 ( k,count0 ) = index;
                count0++;
            }
            else
            {
                S1 ( k,count1 ) = index;
                count1++;
            }
        }
    }


//cout<<"bitmap = " << bitmap <<endl<<endl;
//cout<<"S0="<< S0<<endl<<"S1="<<S1<<endl;


    create_modulation_symbs_list();
}


// private
//
void my_QPSK::create_modulation_symbs_list ( void )
{
    int i,j;
    cvec symbols;

    legi_symbols_list.set_size ( nrof_legi_symbols,false );

    for ( i=0 ; i<nrof_legi_symbols; i++ )
    {
        legi_symbols_list ( i ) = double_complex ( qpsk_matrix ( i,0 ),qpsk_matrix ( i,1 ) );
    }
}

void my_QPSK::modulate_bits ( const bvec &bits, cvec &modulated_symbols )
{
    bvec block;
    int i,j,nrof_blocks,index,nrof_symbs;

    nrof_blocks = floor_i ( bits.length() /block_size );
    nrof_symbs = nrof_blocks;

    modulated_symbols.set_size ( nrof_symbs,false );
    block.set_size ( block_size,false );    // defining the binary vector that will take b bits a time to
    // convert them to an integer index

    for ( i=0; i<nrof_blocks; i++ )
    {
        //Group bits in a binary block of size "block_size"
        for ( j=0; j<block_size; j++ )
        {
            block ( j ) = bits ( i*block_size+j );
        }

        index = bin2dec ( block,false );
        modulated_symbols ( i ) = legi_symbols_list ( index );
    }
}

// ****************** QPSK soft demapper ****************** //
void my_QPSK::demodulate_soft_bits ( const cvec &signal,
                                     vec &channel,
                                     double N0,
                                     vec &L_aposteriori,
                                     vec &L_apriori,
                                     int metric )
{
    int l, i, j, k,m;
    double p_total;
    double P0, P1, d0, d1, P0_log, P1_log, P0_log_approx, P1_log_approx, P0_max, P1_max;
    double d0min, d0min2, d1min, d1min2;
    double treshhold = -log ( DBL_EPSILON ); // To be sure that any precision is left in the calculatation
    double sum_L_a_0, sum_L_a_1;

    vec temp,vec0,vec1;
    bvec bits_0,bits_1, temp_bits_0, temp_bits_1;

    L_aposteriori.set_size ( block_size*signal.size(), false );
    temp.set_size ( 2,false );
    vec0.set_size ( 2,false );
    vec1.set_size ( 2,false );

    // setting the binary temp_bits vectors that will hold the binary bits block by block
    temp_bits_0.set_size ( block_size,false );
    temp_bits_1.set_size ( block_size,false );
    bits_0.set_size ( block_size,false );
    bits_1.set_size ( block_size,false );

    for ( l=0; l<signal.size(); l++ )
    {
//cout<<l<<"  "<<signal(l)<<"   "<<signal.size()<<"  "<<L_apriori.size()<<endl;
        temp ( 0 ) = signal ( l ).real();
        temp ( 1 ) = signal ( l ).imag();

        for ( i=0; i<block_size; i++ )
        {
            P0 = P1 = 0;
            P0_max = P1_max = -1e100;
            P0_log = P1_log = P0_log_approx = P1_log_approx = -100000;

            d0min = d0min2 = d1min = d1min2 = 1e100;

            for ( j=0; j< ( nrof_legi_symbols/2 ); j++ )
            {
                // calculating the required La(x_j)'s

                // first get the binary digits for each legitimate symbol
                temp_bits_0 = dec2bin ( block_size,S0 ( i,j ) );
                temp_bits_1 = dec2bin ( block_size,S1 ( i,j ) );

                //Arranging the binary bits to the correct order
                for ( m=0; m<block_size; m++ )
                {
                    bits_0 ( m ) = temp_bits_0 ( block_size-1-m );
                    bits_1 ( m ) = temp_bits_1 ( block_size-1-m );
                    //bits_0(m)= temp_bits_0(m);
                    //bits_1(m)= temp_bits_1(m);
                }

                sum_L_a_0 = 0.0;
                sum_L_a_1 = 0.0;

                for ( m=0; m<block_size; m++ )
                {
                    /*if (l==0)
                    {
                    cout<<"i="<<i<<endl<<"j="<<j<<endl<<"m="<<m<<endl;
                    cout << "bits_0(m)="<<bits_0(m)<<endl<<"bits_1(m)="<<bits_1(m)<<endl;
                    cout << "L_apriori = "<< L_apriori.get(0,20)<<endl;
                    //cout<<"sum_L_a_0 = "<<sum_L_a_0<<endl<<"sum_L_a_1="<<sum_L_a_1<<endl<<endl;
                    }*/
                    if ( m != i )
                    {
                        if ( bits_0 ( m ) == 1 )
                        {
                            sum_L_a_0 += L_apriori ( l*block_size+m );
                        }
                        if ( bits_1 ( m ) == 1 )
                        {
                            sum_L_a_1 += L_apriori ( l*block_size+m );
                        }
                    }
                    /*if (l==0)
                    {
                    cout<<"sum_L_a_0 = "<<sum_L_a_0<<endl<<"sum_L_a_1="<<sum_L_a_1<<endl<<endl;
                    }*/
                }

                vec0 = get_legi_phasor2 ( S0 ( i,j ) );
                vec1 = get_legi_phasor2 ( S1 ( i,j ) );

                d0 = d1 = 0;
                for ( k=0; k<2; k++ )
                {
                    //d0 += pow((temp(k)-channel(l)*vec0(k)),2.0)/(channel(l)*2*N0); //
                    //d1 += pow((temp(k)-channel(l)*vec1(k)),2.0)/(channel(l)*2*N0); //

                    d0 += pow ( ( temp ( k )-channel ( l ) *vec0 ( k ) ),2.0 ) / ( channel ( l ) *N0 ); //
                    d1 += pow ( ( temp ( k )-channel ( l ) *vec1 ( k ) ),2.0 ) / ( channel ( l ) *N0 ); //
                }

                if ( d0 < d0min )
                {
                    d0min2 = d0min;
                    d0min = d0;
                }
                if ( d1 < d1min )
                {
                    d1min2 = d1min;
                    d1min = d1;
                }

                /*if (l==0)
                {
                cout<< "d0 = "<<d0<<endl<<"d1="<<d1<<endl<<endl;
                //cout<<"sum_L_a_0 = "<<sum_L_a_0<<endl<<"sum_L_a_1="<<sum_L_a_1<<endl<<endl;
                }*/
                if ( metric == EXACT )
                {
                    P0 += exp ( -d0 + sum_L_a_0 ); // EXACT
                    P1 += exp ( -d1 + sum_L_a_1 ); // EXACT
                }
                else if ( metric == LOG )
                {
                    P0_log = jacolog ( P0_log, ( -d0 + sum_L_a_0 ) ); // LOG
                    P1_log = jacolog ( P1_log, ( -d1 + sum_L_a_1 ) ); // LOG
                }
                else if ( metric == LOG_APPROX )
                {
                    P0_log_approx = jacolog_approx ( P0_log_approx, ( -d0 + sum_L_a_0 ) ); // LOG Approx
                    P1_log_approx = jacolog_approx ( P1_log_approx, ( -d1 + sum_L_a_1 ) ); // LOG Approx
                }
                else if ( metric == MAXLOG )
                {
                    P0 = -d0 + sum_L_a_0; // max-log
                    P1 = -d1 + sum_L_a_1; // max-log

                    if ( P0 > P0_max )
                    {
                        P0_max = P0;    // max-log
                    }
                    if ( P1 > P1_max )
                    {
                        P1_max = P1;    // max-log
                    }
                }
                else
                {
                    cout << endl << "ERROR: SPHERE_PACKING::demodulate_soft_bits():" << endl;
                    cout << "undefined metric -- expect 0 or 1 only" << endl;
                    exit ( 0 );
                }
                /*if (l==0)
                {
                cout<< "P0_max = "<<P0_max<<endl<<"P1_max="<<P1_max<<endl<<endl;
                //cout<<"sum_L_a_0 = "<<sum_L_a_0<<endl<<"sum_L_a_1="<<sum_L_a_1<<endl<<endl;
                }*/
            }

            if ( metric == EXACT )
            {
                //normalise propabilities
                p_total = P0+P1;
                P0=P0/p_total;
                P1=P1/p_total;

                //limit the small values
                if ( P0 < 1e-7 ) P0 = 1e-7;
                if ( P1 < 1e-7 ) P1 = 1e-7;

                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) + log ( P1/P0 ); //EXACT
            }
            else if ( metric == LOG )
            {
                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) + P1_log - P0_log; // LOG
            }
            else if ( metric == LOG_APPROX )
            {
                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) + P1_log_approx - P0_log_approx; // LOG Approx

                /*if (l==0)
                    cout<<"L_apriori(l*block_size+i) = "<< L_apriori(l*block_size+i) << endl<< "P1_log_approx =" << P1_log_approx <<endl<< "P0_log_approx="<<P0_log_approx<<endl<<endl<<endl<<endl<<endl<<endl;
                */
            }
            else if ( metric == MAXLOG )
            {
                L_aposteriori ( l*block_size+i ) = L_apriori ( l*block_size+i ) - P0_max + P1_max; // max-log
                /*if (l==0)
                {
                    cout<<"L_apriori(l*block_size+i) = "<< L_apriori(l*block_size+i) << endl;
                    cout<<"L_aposteriori(l*block_size+i) =" << L_aposteriori(l*block_size+i) <<endl<<endl<<endl<<endl<<endl<<endl<<endl;
                }
                */
            }
            else
            {
                cout << endl << "ERROR: SPHERE_PACKING::demodulate_soft_bits():" << endl;
                cout << "undefined metric -- expect 0 or 1 only" << endl;
                exit ( 0 );
            }
        }
    }
}

// get the legitimate phasor coordinates of the given index
vec my_QPSK::get_legi_phasor2 ( int index )
{
    int k;
    vec coordinates;
    coordinates.set_size ( 2,false );

    for ( k=0; k<2; k++ )
    {
        coordinates ( k ) = qpsk_matrix ( index,k );
    }
    return ( coordinates );
}

double my_QPSK::jacolog ( double x, double y )
{
    double r;

    if ( x>y ) r = x + log ( 1 + exp ( y - x ) );
    else    r = y + log ( 1 + exp ( x - y ) );

    return r;
}

double my_QPSK::jacolog_approx ( double x, double y )
{
    double r;
    double diff;

    if ( x>y )
    {
        r = x;
        diff=x-y;
    }
    else
    {
        r = y;
        diff=y-x;
    }

    if ( diff > 3.7 )      r += 0.00;
    else if ( diff > 2.25 ) r += 0.05;
    else if ( diff > 1.5 ) r += 0.15;
    else if ( diff > 1.05 ) r += 0.25;
    else if ( diff > 0.7 ) r += 0.35;
    else if ( diff > 0.43 ) r += 0.45;
    else if ( diff > 0.2 ) r += 0.55;
    else                 r += 0.65;

    return r;
}
