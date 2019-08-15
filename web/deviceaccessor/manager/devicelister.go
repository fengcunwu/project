/*
Copyright 2019 The advantech Qing.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

package manager


import (
	"k8s.io/apimachinery/pkg/api/errors"
	"k8s.io/client-go/tools/cache"
	"k8s.io/apimachinery/pkg/api/meta"	

	"github.com/kubeedge/kubeedge/cloud/pkg/apis/devices/v1alpha1"
)

// DeviceLister helps list and get device
type DeviceLister interface {
	// List lists all device in the indexer for a given namespace.
	List() (ret []*v1alpha1.Device, err error)
	// Get retrieves the device from the indexer for a given id.
	Get(id string) (*v1alpha1.Device, error)
}

//deviceLister implements the DeviceLister
// interface.
type deviceLister struct {
	namespace string
	indexer cache.Indexer
}

// NewDeviceLister return a DeviceLister interface
func NewDeviceLister(ns string, indexer cache.Indexer) DeviceLister {
	return &deviceLister{namespace: ns, indexer: indexer}
}

//List list all devcies in this namespace.
func (dl *deviceLister) List() (ret []*v1alpha1.Device, err error){

	for _, obj :=range dl.indexer.List() {
		meta, err := meta.Accessor(obj)
		if err != nil {
			return nil, err
		}

		if len(meta.GetNamespace()) > 0 {
			if meta.GetNamespace() == dl.namespace {
				ret= append(ret, obj.(*v1alpha1.Device))
			}
		}
    }

	return ret, nil
}

// Get get the device 
func (dl *deviceLister) Get(id string) (*v1alpha1.Device, error){
	obj, exists, err := dl.indexer.GetByKey(dl.namespace + "/" + id)

	if err != nil {
		return nil, err
	}
	if !exists {
		return nil, errors.NewNotFound(v1alpha1.Resource("devices"), id)
	}

	return obj.(*v1alpha1.Device), nil
}
