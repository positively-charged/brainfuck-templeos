/*

   Brainfuck interpreter for TempleOS.

*/

U8 Input() {
   return GetI64( "Brainfuck: enter cell value (000 to 255): ", 0, 0,
      255 ) ( U8 );
}

U0 Err( U8* message, U8* code, I64 ip )$ER$$ER$$ER$ {
   I64 i = 0;
   I64 line = 1;
   I64 column = 0;
   while ( i < ip ) {
      if ( code[ i ] == '\n' ) {
         ++line;
         column = 0;
      }
      else {
         ++column;
      }
      ++i;
   }
   Print( "Brainfuck:%d:%d: error: %s\n", line, column, message );
}

U0 BrainfuckExecute( U8* code ) {
   U8 data[ 30000 ];
   MemSet( data, 0, sizeof( data ) ); 
   I64 ip = 0;
   I64 old_ip = 0;
   I64 dp = 0;
   I64 depth = 0;

   select:
   switch ( code[ ip ] ) {
   case '>': goto gt;
   case '<': goto lt;
   case '+': goto plus;
   case '-': goto minus;
   case '.': goto dot;
   case ',': goto comma;
   case '[': goto lbrac;
   case ']': goto rbrac;
   case '\0':
      finish:
      return;
   // Any other character is ignored.
   default:
      ++ip;
      goto select;
   }

   // Move data pointer to next element.
   gt:
   if ( dp >= sizeof( data ) ) {
      Err( "attempting to set data pointer beyond upper limit", code, ip );
      goto finish;
   }
   ++dp;
   ++ip;
   goto select;

   // Move data pointer to previous element.
   lt:
   if ( ! dp ) {
      Err( "attempting to set data pointer below 0", code, ip );
      goto finish;
   }
   --dp;
   ++ip;
   goto select;

   // Increment element at data pointer.
   plus:
   ++data[ dp ];
   ++ip;
   goto select;

   // Decrement element at data pointer.
   minus:
   --data[ dp ];
   ++ip;
   goto select;

   // Print element at data pointer.
   dot:
   Print( "%c", data[ dp ] );
   ++ip;
   goto select;

   // Read value from user and place it at data pointer.
   comma:
   data[ dp ] = Input();
   ++ip;
   goto select;

   // Go to matching right bracket when element at data pointer is zero.
   lbrac:
   if ( data[ dp ] ) {
      ++ip;
      goto select;
   }
   depth = 0;
   old_ip = ip;
   while ( TRUE ) {
      ++ip;
      switch ( code[ ip ] ) {
      case '[':
         ++depth;
         break;
      case ']':
         if ( depth ) {
            --depth;
         }
         else {
            ++ip;
            goto select;
         }
         break;
      case '\0':
         Err( "missing matching ']'", code, old_ip );
         goto finish;
      }
   }
   // Go back to matching left bracket when element at data pointer is
   // nonzero.
   rbrac:
   if ( ! data[ dp ] ) {
      ++ip;
      goto select;
   }
   depth = 0;
   old_ip = ip;
   while ( TRUE ) {
      if ( ! ip ) {
         Err( "missing matching '['", code, old_ip );
         goto finish;
      }
      --ip;
      switch ( code[ ip ] ) {
      case ']':
         ++depth;
         break;
      case '[':
         if ( depth ) {
            --depth;
         }
         else {
            ++ip;
            goto select;
         }
      }
   }
}