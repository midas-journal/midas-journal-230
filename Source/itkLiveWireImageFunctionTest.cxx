#include <string>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLiveWireImageFunction.h"
#include "itkPathIterator.h"

int itkLiveWireImageFunctionTest0( int argc, char *argv[] )
{
  typedef float PixelType;
  const unsigned int ImageDimension = 2;
  typedef itk::Image<PixelType, ImageDimension> ImageType;
  typedef itk::Image<unsigned char, ImageDimension> LabelImageType;   

  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] ); 
  reader->Update();

  LabelImageType::Pointer labelImage = LabelImageType::New();
  labelImage->SetOrigin( reader->GetOutput()->GetOrigin() );
  labelImage->SetSpacing( reader->GetOutput()->GetSpacing() );
  labelImage->SetRegions( reader->GetOutput()->GetLargestPossibleRegion() );
  labelImage->Allocate();
  labelImage->FillBuffer( 0 );

  ImageType::IndexType anchor;
  ImageType::IndexType free;
  for ( unsigned int d = 0; d < ImageDimension; d++ )
    {
    anchor[d] = atoi( argv[d+3] );
    free[d] = atoi( argv[d+3+ImageDimension] );
    } 

  typedef itk::LiveWireImageFunction<ImageType> LiveWireType;
  LiveWireType::Pointer livewire = LiveWireType::New();
  typedef LiveWireType::OutputType PathType;

  livewire->SetInputImage( reader->GetOutput() );
  livewire->SetAnchorSeed( anchor );
  livewire->SetInputImage( reader->GetOutput() );
  
  livewire->SetGradientMagnitudeWeight( 0.43 );
  livewire->SetZeroCrossingWeight( 0.43 );
  livewire->SetGradientDirectionWeight( 0.14 ); 
  livewire->SetUseImageSpacing( true );
  livewire->SetUseFaceConnectedness( true );

  if ( argc > 7 )
    {
    typedef itk::ImageFileReader<LiveWireType::MaskImageType> MaskReaderType;
    MaskReaderType::Pointer maskReader = MaskReaderType::New();
    maskReader->SetFileName( argv[7] );
    maskReader->Update();
    
    livewire->SetMaskImage( maskReader->GetOutput() );
    livewire->SetInsidePixelValue( 1 );
    } 

  LabelImageType::PixelType pathLabel = 1;

  if ( PathType::Pointer path = livewire->EvaluateAtIndex( free ) )
    {
				typedef itk::PathIterator<LabelImageType, PathType> IteratorType;
				IteratorType It( labelImage, path );
				It.GoToBegin();
				while ( !It.IsAtEnd() )
						{
						labelImage->SetPixel( It.GetIndex(), pathLabel );
						++It;
						}
    }   

  typedef itk::ImageFileWriter<LabelImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[2] );
  writer->SetInput( labelImage );
  writer->Update();

  return EXIT_SUCCESS;
}

int main( int argc, char *argv[] )
{
  if ( argc < 7 )
    {
    std::cout << "Usage: " << argv[0] << " inputImage outputImage "
              << "anchorSeed[0] anchorSeed[1] index[0] index[1] "
              << "[maskImage]" << std::endl;
    return EXIT_FAILURE;
    }
    
  int test = EXIT_FAILURE;  
  test = itkLiveWireImageFunctionTest0( argc, argv );

  return test;  
}