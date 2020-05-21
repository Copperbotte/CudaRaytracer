#include "matrix.h"

#include <string>

matrix::matrix() :cols(0), rows(0), data(nullptr) { ; }

matrix::~matrix()
{
	cols = rows = 0;
	delete[] data;
	data = nullptr;
}

matrix::matrix(const matrix& copy):cols(copy.cols),rows(copy.rows)
{
	data = new float[cols * rows];
	memcpy(data, copy.data, cols * rows * sizeof(float));
}

matrix::matrix(int _cols, int _rows, const float* _data):cols(_cols),rows(_rows),data(nullptr)
{
	data = new float[cols * rows];
	if (_data)
		memcpy(data, _data, cols * rows * sizeof(float));
	else
		for (int i = 0; i < cols * rows; ++i)
			data[i] = 0.0f;
}

int matrix::getRows() const
{
	return rows;
}

int matrix::getCols() const
{
	return cols;
}

const float* matrix::getDataPtr() const
{
	return data;
}

float matrix::getData(int c, int r) const
{
	//column major matrices, for easy vectors
	if (r < 0 || c < 0 || cols <= c || rows <= r)
	{
		std::string e_message("Matrix access out of range! ");
		e_message += "Cols:" + std::to_string(cols) + ", Rows:" + std::to_string(rows) + " ";
		e_message += "Accessed [" + std::to_string(c) + ", " + std::to_string(r) + "]\n";
		throw std::out_of_range(e_message);
		return 0.0f;
	}

	return data[r * cols + c];
}

float matrix::setData(int c, int r, float d)
{
	//column major matrices, for easy vectors
	if (c < 0 || r < 0 || cols <= c || rows <= r)
	{
		std::string e_message("Matrix access out of range! ");
		e_message += "Cols:" + std::to_string(cols) + ", Rows:" + std::to_string(rows) + " ";
		e_message += "Accessed [" + std::to_string(c) + ", " + std::to_string(r) + "]\n";
		throw std::out_of_range(e_message);
		return 0.0f;
	}

	data[r * cols + c] = d;

	return d;
}

matrix& matrix::operator=(const matrix& R)
{
	//i wish this worked
	//delete this;
	//matrix(R);
	cols = R.cols;
	rows = R.rows;
	delete[] data;
	data = new float[cols * rows];
	memcpy(data, R.data, cols * rows * sizeof(float));
	return *this;
}

matrix matrix::operator-(void) const
{
	matrix M(*this);
	for (int i = 0; i < cols * rows; ++i)
		M.data[i] *= -1;
	return M;
}

matrix& matrix::operator+=(const matrix& R)
{
	bool e_cols = cols != R.cols;
	bool e_rows = rows != R.rows;
	if (e_cols || e_rows)
	{
		std::string e_message("Add domain mismatch! ");
		e_message += "Cols[ L:" + std::to_string(cols) + ", R:" + std::to_string(R.cols) + "] ";
		e_message += "Rows[ L:" + std::to_string(rows) + ", R:" + std::to_string(R.rows) + "]\n";
		throw std::length_error(e_message);
		return *this;
	}
	
	for (int i = 0; i < cols * rows; ++i)
		data[i] += R.data[i];

	return *this;
}

matrix& matrix::operator-=(const matrix& R)
{
	return *this += -R;
}

matrix& matrix::operator*=(float f)
{
	for (int i = 0; i < cols * rows; ++i)
		data[i] *= f;
	return *this;
}

matrix& matrix::operator*=(const matrix& R)
{
	return *this = *this * R;
}

matrix& matrix::operator/=(float f)
{
	for (int i = 0; i < cols * rows; ++i)
		data[i] /= f;
	return *this;
}

matrix matrix::operator+(const matrix& R) const
{
	return matrix(*this) += R;
}

matrix matrix::operator-(const matrix& R) const
{
	return matrix(*this) -= R;
}

matrix matrix::operator*(const matrix& R) const
{
	bool e_mul = cols != R.rows;
	if (e_mul)
	{
		std::string e_message("Multiply domain mismatch! ");
		e_message += "Cols[ L:" + std::to_string(cols) + ", R:" + std::to_string(R.cols) + "] ";
		e_message += "Rows[ L:" + std::to_string(rows) + ", R:" + std::to_string(R.rows) + "]\n";
		throw std::length_error(e_message);
		return *this;
	}

	//initialized as zero
	matrix M(R.cols, rows, nullptr);

	for (int c = 0; c < M.cols; ++c)
		for (int r = 0; r < M.rows; ++r)
			for (int n = 0; n < cols; ++n)
				M.setData(c, r, M.getData(c, r) + getData(n, r) * R.getData(c, n));

	return M;
}
