#define check_time  2
#define ping_intrvl 1

void pingThread()
{
  if(time() > next_ping)
  {
   next_ping = time() + pint_intrvl;
   std::map<char, int> neighbours = rt.getNeighbours();
   for (std::map<char, int>::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
   {
    send(it->first, 'P');
   }
  }
  
  if(time() > check_time)
  {
    check_time = time() + check_time;
    std::map<char, int> neighbours = rt.getNeighbours();
    for (std::map<char, int>::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
    {
      int index = it->first - 'A' //sub char A from id to get index
      if (active[index] == 1) 
      {
        active[index] == 0;
      }
      else
      {
        rt.setUnreachable(it->first); 
      }
    }
  }
}

//src, dst, ack_request = 'p'
