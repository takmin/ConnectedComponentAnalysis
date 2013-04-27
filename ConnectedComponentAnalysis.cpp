// Connected Component Analysis
#include <opencv2/core/core.hpp>

//! Connected Component Analysis Function with 8 neighbors
template<typename T> void ConnectedComponentAnalysisBase(const cv::Mat& src_mat, std::vector<std::vector<cv::Point>>& connected_components)
{
	int x,y;
	int i,k,m;

	int rows = src_mat.rows;
	int cols = src_mat.cols;
	T *src_data = (T*)src_mat.data;

	cv::Mat cc_label_map = cv::Mat::zeros(rows, cols, CV_32SC1);
	int* cc_label_ptr = (int*)cc_label_map.data;

	int neighbor_label[4];
	int neighbor_pos[4];
	neighbor_pos[0] = cols + 1;
	neighbor_pos[1] = cols;
	neighbor_pos[2] = cols - 1;
	neighbor_pos[3] = 1;

	int label;
	int prev_label;
	T val;
	std::vector<int> label_tbl;

	// If the neighbor of the target already has a label, mark the target as the same label
	// If the neighbors have differenct labels, record it to label_tbl
	for(y=0;y<rows;y++){
		for(x=0;x<cols;x++){
			int offset = y*cols + x;
			if(src_data[offset] > 0){
				val = src_data[offset];
				neighbor_label[0] = (y>0 && x>0 && src_data[offset-neighbor_pos[0]]==val) ? cc_label_ptr[offset - neighbor_pos[0]] : 0;
				neighbor_label[1] = (y>0 && src_data[offset-neighbor_pos[1]]==val) ? cc_label_ptr[offset - neighbor_pos[1]] : 0;
				neighbor_label[2] = (y>0 && x<cols-1 && src_data[offset-neighbor_pos[2]]==val) ? cc_label_ptr[offset - neighbor_pos[2]] : 0;
				neighbor_label[3] = (x>0 && src_data[offset-neighbor_pos[3]]==val) ? cc_label_ptr[offset - neighbor_pos[3]] : 0;
				if(!neighbor_label[0] && !neighbor_label[1] && !neighbor_label[2] && !neighbor_label[3]){
					label = label_tbl.size();
					label_tbl.push_back(label);
					label++;
				}
				else{
					label = 0;
					for(i=0;i<4;i++){
						prev_label = neighbor_label[i];
						if(prev_label){
							if(!label){
								label = prev_label;
							}
							else if(prev_label!=label){
								k = label - 1;
								m = prev_label - 1;
								while(label_tbl[k]<k){
									k=label_tbl[k];
								}
								while(label_tbl[m]<m){
									m=label_tbl[m];
								}
								if(m<k){
									label_tbl[k] = m;
									label = m+1;
								}else{
									label_tbl[m] = k;
									label = k+1;
								}
							}
						}
					}
				}
				cc_label_ptr[offset] = label;
			}
		}
	}

	// integrate labels recorded in label_tbl
	int cc_count = 0;
	std::vector<int> cc_id(label_tbl.size(),0);
	for(i=label_tbl.size()-1; i>=0; i--){
		if(label_tbl[i] == i){
			cc_count++;
			cc_id[i] = cc_count;
		}
		else{
			m = i;
			while(label_tbl[m]<m){
				m=label_tbl[m];
			}
			label_tbl[i] = m;
		}
	}

	
	// Store the final result
	cc_label_ptr = (int*)cc_label_map.data;
	connected_components.resize(cc_count);
	for(y=0;y<rows;y++){
		for(x=0;x<cols;x++){
			if(*cc_label_ptr > 0){
				int id = cc_id[label_tbl[*cc_label_ptr-1]]-1;
				connected_components[id].push_back(cv::Point(x,y));
			}
			cc_label_ptr++;
		}
	}
}


//! Connected Component Analysis Function with 8 neighbors
/*!
\input[in] src_mat input image which can be CV_8UC1, CV_32SC1, CV_32FC1, CV_64FC1
\param[out] connected_components all pixel coordinates in connected components
*/
void ConnectedComponentAnalysis(const cv::Mat& src_mat, std::vector<std::vector<cv::Point>>& connected_components)
{
	switch(src_mat.type()){
	case CV_8UC1:
		ConnectedComponentAnalysisBase<unsigned char>(src_mat, connected_components);
		break;
	case CV_32SC1:
		ConnectedComponentAnalysisBase<int>(src_mat, connected_components);
		break;
	case CV_32FC1:
		ConnectedComponentAnalysisBase<float>(src_mat, connected_components);
		break;
	case CV_64FC1:
		ConnectedComponentAnalysisBase<double>(src_mat, connected_components);
		break;
	}
}

