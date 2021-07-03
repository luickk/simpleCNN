#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*activationFunc)(float inp, float out);

enum layerType {
    fullyConnected,
};

typedef struct {
  float *bias;
  float *weights;
  float *nodes;
  activationFunc actFunc;
  int size;
  layerType type;
} baseLayer;

typedef struct {
  int nLayer;
  baseLayer *nnLayer[];
} neuralNet;

/*
general NN functions
*/
void initLayer(float size, layerType type, baseLayer &layer, activationFunc actFunc) {
  layer.bias = (float*)malloc(size * sizeof(float));
  layer.weights = (float*)malloc(size * sizeof(float));
  layer.nodes = (float*)malloc(size * sizeof(float));

  layer.actFunc = actFunc;
  layer.size = size;
  layer.type = type;
}

baseLayer *createLayer(float size, layerType type, activationFunc actFunc) {
  static baseLayer layer;
  initLayer(size, fullyConnected, layer, actFunc);
  return &layer;
}

void initWeights(baseLayer *layer, int size){
  int randW = 0;
  for (int i = 0; i < size; i++) {
    randW = (rand() % (10 - 0 + 1)) + 0;
    layer->weights[i] = randW;
  }
}

void initBias(baseLayer *layer, int size){
  int randB = 0;
  for (int i = 0; i < size; i++) {
    randB = (rand() % (2 - 0 + 1)) + 0;
    layer->bias[i] = randB;
  }
}

neuralNet *createNet(baseLayer *layer[], int nLayer) {
  neuralNet *nn = (neuralNet *)malloc(sizeof(baseLayer)*nLayer+sizeof(nLayer));
  for (int i=0; i<nLayer; ++i) {
     nn->nnLayer[i]=layer[i];
  }
  nn->nLayer = nLayer;

  for (int i=0; i<nLayer; ++i) {
    initWeights(nn->nnLayer[i],nn->nnLayer[i]->size);
    initBias(nn->nnLayer[i],nn->nnLayer[i]->size);
  }

  return nn;
}

/*
Layer Operations
*/
void testActFunc(float inp, float out){
  if (inp < 1) {
    out = 0;
  } else if (inp >=1) {
    out = 1;
  }
}

void calcNodeValForLayer(baseLayer *lastLayer, baseLayer *layer) {
  for(int i = 0; i<layer->size; i++) {
    for (int j = 0; j<lastLayer->size; j++) {
      layer->nodes[i] += layer->weights[i] * lastLayer->nodes[j];
      printf("%i, %i: %.2f \n", i, j, layer->nodes[i]);
    }
  }
}

void feedForward(neuralNet *net, float input[4], int nPredict){
  // setting input to input layer nodes
  for (int i = 0; i < nPredict; i++) {
    net->nnLayer[0]->nodes[i] = input[i];
  }

  for(int i = 1; i < net->nLayer; i++) {
    calcNodeValForLayer(net->nnLayer[i-1], net->nnLayer[i]);
  }
}

/*
util functions
*/
int trainDNN(int nPredict, neuralNet *net, char pathToFile[]) {
  FILE *fp;
  char *line = 0;
  float inp[4] = {0};
  int inpIt = 0;
  size_t len = 0;
  ssize_t read;

  fp = fopen(pathToFile, "r");
  if (fp == 0) {
   return 1;
  }
  while ((read = getline(&line, &len, fp)) != -1) {
    if (inpIt == nPredict) {
      // printf("\n ");
      // for(int j = 0; j < nPredict; j++) {
        // printf("%f ", inp[j]);
      // }
      feedForward(net, inp, nPredict);
      inpIt = 0;
      break;
    }
    inp[inpIt] = atof(line);
    inpIt++;
  }
  fclose(fp);
  if (line) {
   free(line);
  }
  return 0;
}

int main(){
  int nPredict = 4;

  printf("nPredict: %d \n", nPredict);

  baseLayer *inpLayer = createLayer(nPredict, fullyConnected, testActFunc);
  baseLayer *hiddenLayer2 = createLayer(100, fullyConnected, testActFunc);
  baseLayer *hiddenLayer1 = createLayer(100, fullyConnected, testActFunc);
  baseLayer *outpLayer = createLayer(nPredict, fullyConnected, testActFunc);

  baseLayer *layer[] = {inpLayer, hiddenLayer1, hiddenLayer2, outpLayer};
  neuralNet *dnn = createNet(layer, 4);

  // dnn = {4, layer};
  int rc = trainDNN(nPredict, dnn, "../data/datasetByLine.csv");
  if (rc != 0) {
     printf("file open failed");
     return 0;
  }
  for (int i = 0; i < nPredict; i++) {
    printf("res: %.4f \n", outpLayer->nodes[i]);
  }
}
