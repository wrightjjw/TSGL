#include <VisualTaskQueue.h>

namespace tsgl {

 /*!
  * \brief Default VisualTaskQueue constructor method.
  * \details This is the default constructor for the VisualTaskQueue class.
  *   \param elements The maximum number of elements to be drawn on the VisualTaskQueue. Setting this to higher than
  *     the actual number of elements may result in some unused, empty rectangle. Setting this to lower than
  *     the actual number of elements may result in some rectangles being drawn off the VisualTaskQueue Canvas.
  *   \param sideLength The side length in pixels of the task rectangles to be drawn on the VisualTaskQueue Canvas.
  *   \param aspec The approximate aspect ratio of height/width for the VisualTaskQueue Canvas.
  *   \param spacing The space in pixels between the rectangles representing elements in the VisualTaskQueue.
  *   \param borderLength The space in pixels between the outer VisualTaskQueue rectangles and the border of the
  *     VisualTaskQueue Canvas.
  * \return A new VisualTaskQueue with the specified maximum number of elements, rectangle side length,
  *   approximate aspect ratio, spacing between rectangles, and spacing around the borders.
  */
VisualTaskQueue::VisualTaskQueue(int elements, int sideLength, float aspect, int spacing, int borderLength) {
  showingLegend = false;
  space = spacing;
  border = borderLength;
  totalElements = elements;
  rowLength = ceil(sqrt(totalElements/aspect));  //Number of elements per row
  blockSize = sideLength;
  vcan = new Canvas(0,-1,2*border+(blockSize+space)*rowLength,2*border+(blockSize+space)*elements/rowLength,"Thread colors");
  vcan->start();
  // reset();
}

 /*!
  * \brief VisualTaskQueue destructor method.
  * \details This is the destructor for the VisualTaskQueue class.
  * \details Frees up memory that was allocated to a VisualTaskQueue instance.
  */
VisualTaskQueue::~VisualTaskQueue() {
  delete vcan;
  if (showingLegend)
    delete lcan;
}

 /*!
  * \brief Shows a key/legend for the VisualTaskQueue.
  * \details This function opens up a separate Canvas displaying a legend for the VisualTaskQueue, showing which
  *   colors correspond to which threads.
  *   \param threads The number of threads the VisualTaskQueue is using.
  */
void VisualTaskQueue::showLegend(int threads) {
  showingLegend = true;
  const int TEXTW = 24, GAP = 4;
  if (threads == -1)
    threads = omp_get_num_threads();

  //Ugly calculations :(
  int offset = border+space;
  int xStart = border;
  int xDelta = TEXTW*2;
  int yStart = TEXTW + offset;
  int yDelta = blockSize+space;
  int oheight = vcan->getWindowHeight();
  int myHeight = TEXTW + (threads+1) * yDelta;
  if (myHeight > oheight)
    myHeight = oheight;
  int perColumn = (myHeight-yStart)/yDelta;
  int yCutoff = yStart + yDelta*perColumn-blockSize;
  int myWidth = 2*border + ((threads)/perColumn)*xDelta+blockSize+TEXTW;
#ifdef _WIN32
if (myWidth < 116);  //Magic number for Windows windows...
myWidth = 116;
#endif

//   //Actually draw things
  lcan = new Canvas(vcan->getWindowX()+vcan->getWindowWidth(),vcan->getWindowY(),myWidth,myHeight,"");
  lcan->start();
  lcan->getBackground()->drawText(0,lcan->getWindowHeight()/2-TEXTW/2,0,"Legend:","./assets/freefont/FreeSerif.ttf",TEXTW,0,0,0,BLACK);
  int xx = -lcan->getWindowWidth()/2 + xStart, yy = lcan->getWindowHeight()/2 - yStart;
  for (int i = 0; i < threads; ++i) {
    lcan->getBackground()->drawSquare(xx,yy,0,blockSize,0,0,0,Colors::highContrastColor(i));
    lcan->getBackground()->drawText(xx+blockSize+GAP,yy,0,std::to_wstring(i),"./assets/freefont/FreeSerif.ttf",TEXTW/2,0,0,0,BLACK);
    yy -= yDelta;
    if (yy > yCutoff) {
      yy = yStart;
      xx += xDelta;
    }
  }
}

 /*!
  * \brief Updates the state of the VisualTaskQueue.
  * \details This function updates an element of the VisualTaskQueue with a new state.
  *   \param index The index of the element to update.
  *   \param state The new state to put the element in. Must be one of RUNNING or FINISHED.
  */
void VisualTaskQueue::update(int index, VQState state) {
  int x = index % rowLength;
  int y = index / rowLength;
  vcan->getBackground()->drawSquare(
    border*2+x*(blockSize+space)-vcan->getWindowWidth()/2,vcan->getWindowHeight()/2 - (border * 1.5+y*(blockSize+space)),0,
    blockSize,
    0,0,0,
    Colors::blend(
      Colors::highContrastColor(omp_get_thread_num()),(state == RUNNING) ? BLACK : WHITE,0.5f
    )
  );
}

 /*!
  * \brief Resets all of the elements in the VisualTaskQueue.
  * \details This function tells VisualTaskQueue to clear the state information of all of the elements.
  */
void VisualTaskQueue::reset() {
  for (int i = 0; i < totalElements; ++i) {
    int x = i % rowLength;
    int y = i / rowLength;
    vcan->getBackground()->drawSquare(
      border*2+x*(blockSize+space)-vcan->getWindowWidth()/2,vcan->getWindowHeight()/2 - (border * 1.5+y*(blockSize+space)),0,
      blockSize,
      0,0,0,
      WHITE
    );
  }
}

 /*!
  * \brief Closes the visual queue.
  * \details This function closes and destroys the internal Canvas created by the VisualTaskQueue.
  * \warning <b> Do not attempt to reset() or update() the VisualTaskQueue after closing it.</b>
  */
void VisualTaskQueue::close() {
  if (showingLegend) {
    if (lcan->isOpen())
      lcan->close();
    lcan->wait();
  } 
  if (vcan->isOpen())
    vcan->close();
  vcan->wait();
}

void VisualTaskQueue::sleep() {
  if (showingLegend)
    lcan->sleep();
  vcan->sleep();
}

}
