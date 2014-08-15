void propagate(float *input, float *output) {
  output[0] = ((((((input[0]) > 0.f) ? 1.f : -1.f) * 0.125 + (((input[1]) > 0.f) ? 1.f : -1.f) * 0.5) > 0.f) ? 1.f : -1.f) * -0.125 + ((((((input[0]) > 0.f) ? 1.f : -1.f) * -0.5 + (((input[1]) > 0.f) ? 1.f : -1.f) * -0.5) > 0.f) ? 1.f : -1.f) * -1 + ((((((input[0]) > 0.f) ? 1.f : -1.f) * 1 + (((input[1]) > 0.f) ? 1.f : -1.f) * 0) > 0.f) ? 1.f : -1.f) * -0.875;
}

int main()
{
  float input[2];
  float output;

  input[0] = 0;
  input[1] = 0;
  propagate(input, &output);
  printf("%f ^ %f = %f\n", input[0], input[1], output);

  input[0] = 0;
  input[1] = 1;
  propagate(input, &output);
  printf("%f ^ %f = %f\n", input[0], input[1], output);

  input[0] = 1;
  input[1] = 0;
  propagate(input, &output);
  printf("%f ^ %f = %f\n", input[0], input[1], output);

  input[0] = 1;
  input[1] = 1;
  propagate(input, &output);
  printf("%f ^ %f = %f\n", input[0], input[1], output);

  return 0;
}
